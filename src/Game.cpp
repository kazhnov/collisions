#include "Game.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Variables.hpp"
#include "Chunk.hpp"
#include "ItemEntity.hpp"
#include "NPC.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <raylib.h>
#include <sol/forward.hpp>
#include <thread>
#include <type_traits>
#include <utility>
#include <random>

static std::random_device dev;
static std::mt19937 rng(dev());
static std::uniform_real_distribution<> random_value (0.f, 1.f); // distribution in range [1, 6]

/*
void Game::initialize() {
    tiles = new Tile[sizeY * sizeX];
    for (uint y = 0; y < sizeY; y++) {

        for (uint x = 0; x < sizeX; x++) {
            Tile *tile = getTileptr(Vector2{(float)x, (float)y});
            tile->setType("void");
            tile->setPos(x, y);
        }
    }
}
Tile *Game::getTileptr(Vector2 pos) {
    pos.x += 0.5f;
    pos.y += 0.5f;
    uint x = pos.x;
    uint y = pos.y;
    if(x >= sizeX || y >= sizeY) {
        return nullptr;
    }
    return tiles + y*sizeX + x;
}
*/

static std::mutex mutex;

Game::Game(Player &player, Display &display) : player(player), display(display) {
    this->player = player;
    std::cout << "world created" << std::endl;
    std::cout << "world initialized" << std::endl;
    Variables::lua["game"] = this;
}

Chunk* Game::loadChunk(int x, int y) {
    //std::lock_guard<std::mutex>lock(chunkMutex);
    //std::lock_guard<std::mutex> guard(Variables::mutex);
    return new Chunk(x, y);
}

void Game::updateChunks() {
    Chunk* center = getChunkptrFromPos(player.getPos());    
    std::vector<Vector2> chunkPos{};
    Vector2 centerPos = center->getPos();
    //std::cout <<centerPos.x << "\t" << centerPos.y << std::endl;

    auto it = chunks.begin();
    while (it !=chunks.end()){
        if (
            std::abs(centerPos.x - it->get()->getPos().x) > Variables::RenderDistance ||
            std::abs(centerPos.y - it->get()->getPos().y) > Variables::RenderDistance
        ) {
            it->get()->save();
            it = chunks.erase(it);
        }
        else ++it;
    }
    
    for (auto &chunk: chunks) {
        chunkPos.push_back(chunk->getPos());
    }
    //std::cout << std::this_thread::get_id() << std::endl;
    std::vector<std::future<Chunk*>> chunkFutures{};
    for (int i = -Variables::RenderDistance; i <= Variables::RenderDistance; i++) {
        for (int j = -Variables::RenderDistance; j <= Variables::RenderDistance; j++) {
            int x = centerPos.x + j;
            int y = centerPos.y + i;
            if (std::find_if(chunkPos.begin(), chunkPos.end(), [x, y](Vector2 pos){
                return pos.x == x && pos.y == y;
            }) == chunkPos.end()) {
                //std::cout << x << "\t" << y << std::endl;

                chunkFutures.push_back(std::async(std::launch::async, loadChunk, x, y));

                //chunks.push_back(Chunk(x, y));
            }
        }
    }
    for (auto &chunkF : chunkFutures) {
        Chunk *chunk = chunkF.get();
        chunk->initialize();
        chunks.push_back(std::unique_ptr<Chunk>(chunk));
        //std::cout << "loaded chunk " << chunks.size() << std::endl;
    }
    chunkFutures.clear();
    //std::cout << "update finished" <<std::endl;
    std::sort(chunks.begin(), chunks.end(),[](std::unique_ptr<Chunk>& a, std::unique_ptr<Chunk>& b){
        return a->getPos().y < b->getPos().y;
    });
}
        
bool Game::putTile(Vector2 pos, std::string id) {
    Tile *old = getTileptr(pos);
    if (old == nullptr) return false;
    std::string oldid = old->getType()->id;
    if (oldid == id) return false;
    Tile newt = Tile(*old);
    newt.setType(id);

    bool typeset{}, onEnter{}, onLeave{}, onChange = true;


    if (player.collider.isColliding(old->getHitbox())) {
        if(old->getType()->isWalkable) {
            onLeave = true;
        } else {
            onChange = false;
        }
    }


    if (player.collider.isColliding(newt.getHitbox())) {
        if (newt.getType()->isWalkable) {
            onEnter = true;
        } else {
            onChange = false;
        }
    }
    
    if (!onChange) return false;

    old->onBreak();
    old->setType(id);
    return true;
}

void Game::draw() {
    Variables::toDraw.push_back(&player);
    for (auto entity : entities) {
        Variables::toDraw.push_back(entity);
    }
    for (auto &chunk : chunks) {
        chunk->drawFloors();
    }
    for (auto &chunk : chunks) {
        chunk->drawTiles();
    }
    Variables::toDraw.clear();
}

Chunk *Game::getChunkptrFromPos(Vector2 pos) {
    int x = std::floor(pos.x);
    int y = std::floor(pos.y);
    x = std::floor(x / (float)CHUNKSIZE);
    y = std::floor(y / (float)CHUNKSIZE);
    return getChunkptr(x, y);

}

Chunk *Game::getChunkptr(int x, int y) {
    for (size_t i = 0; i < chunks.size(); i++) {
        if (chunks[i]->getPos() == Vector2{(float)x, (float)y}) {
            return chunks[i].get();
        }
    }
    chunks.push_back(std::make_unique<Chunk>(x,y));
    return (chunks.end()-1)->get();
    assert(false);
}

Tile *Game::getTileptr(Vector2 pos) {
    Chunk *chunk = getChunkptrFromPos(pos);
    return chunk->getTileptr(pos);
}

Floor *Game::getFloorptr(Vector2 pos) {
    Chunk *chunk = getChunkptrFromPos(pos);
    return chunk->getFloorptr(pos);
}

void Game::initLua() {
    Variables::lua.new_usertype<Game>("Game",
        "putTile", &Game::putTile,
        "getTile", &Game::getTileptr
    );
}

void Game::save() {
    for (auto &chunk : chunks) {
        chunk->save();
    }
}

std::optional<Item> Game::breakTile(Vector2 pos) {
    Tile* tile = getTileptr(pos);
    if (tile->getType()->id == "void") {
        return std::nullopt;
    }
    tile->onBreak();
    std::string id = tile->getType()->id;
    tile->setType("void");
    tile->onCreate();
    return Item(id, 1);
}

std::vector<Entity*> &Game::getEntities() {
    return entities;
}

std::vector<Entity *> Game::getEntityFromPos(Vector2 pos, float radius) {
    std::vector<Entity *> near{};
    for (auto &entity : entities) {
        if (Vector2DistanceSqr(entity->getPos(), pos) < radius*radius) {
            near.push_back(entity);
        }
    }
    return near;
}

void Game::update(double delta) {
    for (auto *entity : entities) {
        if (auto mob = dynamic_cast<NPC*>(entity)) {
            mob->calculateRoute();
        }
        if (auto item = dynamic_cast<ItemEntity*>(entity)) {
            item->accelerateTowards({}, delta, 10);
        }
        entity->moveAndCollide(delta);
    }
    player.moveAndCollide(delta);
    player.cooldown -= (float) delta;
    player.cooldown = std::max(player.cooldown, 0.f);
    auto near = getEntityFromPos(player.getPos(), 2.f);
    std::vector<size_t> toDelete;
    for (auto &entity : near) {
        if (auto item = dynamic_cast<ItemEntity*>(entity)){
            if (!item->getCollider()->isColliding(player.collider)){
                player.suckEntity(entity);
                continue;
            };

            player.addItem(Item(item->item.getType()->id, item->item.count));
            auto found = std::find_if(entities.begin(), entities.end(), [entity](Entity* ent) {
                return ent == entity;
            });
            if (found != entities.end()) {
                toDelete.push_back(found-entities.begin());
                *found = nullptr;
            }
            delete item;
        }
    }

    auto it = entities.begin();
    while (it != entities.end()) {
        if (*it == nullptr) {
            it = entities.erase(it);
        } else ++it;
    }
}

void Game::addItemEntity(Item item, Vector2 pos) {
    ItemEntity *entity = new ItemEntity(item, pos);
    entity->setVel({
        ((float)random_value(rng)-0.5f) * 5,
        ((float)random_value(rng)-0.5f) * 5
    });
    entities.push_back(entity);
}

void Game::addNPC(NPC npc) {
    NPC *entity = new NPC(npc);
    entities.push_back(entity);
}

Game::~Game(){
    for (auto entity: entities) {
        if (auto mob = dynamic_cast<NPC*>(entity)){
            delete mob;
        }
        else if (auto item = dynamic_cast<ItemEntity*>(entity)){
            delete item;
        }
    }
};

