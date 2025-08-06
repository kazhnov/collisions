#include "Game.hpp"
#include "Variables.hpp"
#include <algorithm>
#include <cassert>
#include <raylib.h>
#include <sol/forward.hpp>
#include <type_traits>

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

void Game::updateChunks() {
    Chunk* center = getChunkptrFromPos(player.getPos());    
    std::vector<Vector2> chunkPos{};
    Vector2 centerPos = center->getPos();
    //std::cout <<centerPos.x << "\t" << centerPos.y << std::endl;
    std::vector<Chunk>::iterator it = chunks.begin();
    while (it !=chunks.end()){
        if (
            std::abs(centerPos.x - it->getPos().x) > Variables::RenderDistance ||
            std::abs(centerPos.y - it->getPos().y) > Variables::RenderDistance
        ) {
            it->save();
            it = chunks.erase(it);
        }
        it++;
    }
    
    for (auto &chunk: chunks) {
        chunkPos.push_back(chunk.getPos());
    }
    for (int i = -Variables::RenderDistance; i <= Variables::RenderDistance; i++) {
        for (int j = -Variables::RenderDistance; j <= Variables::RenderDistance; j++) {
            int x = centerPos.x + j;
            int y = centerPos.y + i;
            if (std::find_if(chunkPos.begin(), chunkPos.end(), [x, y](Vector2 pos){
                return pos.x == x && pos.y == y;
            }) == chunkPos.end()) {
                //std::cout << x << "\t" << y << std::endl;
                chunks.push_back(Chunk(x, y));
            }
        }
    }
}

void Game::putTile(Vector2 pos, std::string id) {
    Tile *old = getTileptr(pos);
    if (old == nullptr) return;
    std::string oldid = old->getType()->id;
    if (oldid == id) return;
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
    
    if (!onChange) return;

    Variables::lua["TileScripts"][oldid]["onDelete"](old);
    old->setType(id);
    Variables::lua["TileScripts"][id]["onCreate"](old);
}

void Game::draw() {
    for (auto &chunk : chunks) {
        chunk.draw();
    }
}

Chunk *Game::getChunkptrFromPos(Vector2 pos) {
    pos.x += 0.5f;
    pos.y += 0.5f;
    int x = std::floor(pos.x);
    int y = std::floor(pos.y);
    x = std::floor(x / (float)CHUNKSIZE);
    y = std::floor(y / (float)CHUNKSIZE);
    return getChunkptr(x, y);

}

Chunk *Game::getChunkptr(int x, int y) {
    for (int i = 0; i < chunks.size(); i++) {
        if (chunks[i].getPos() == Vector2{(float)x, (float)y}) {
            return chunks.data() + i;
        }
    }
    Chunk chunk(x, y);
    chunks.push_back(chunk);
    for (int i = 0; i < chunks.size(); i++) {
        if (chunks[i].getPos() == Vector2{(float)x, (float)y}) {
            return chunks.data() + i;
        }
    }
    assert(false);
}

Tile *Game::getTileptr(Vector2 pos) {
    Chunk *chunk = getChunkptrFromPos(pos);
    return chunk->getTileptr(pos);
}

void Game::initLua() {
    Variables::lua.new_usertype<Game>("Game",
        "tiles", &Game::tiles,
        "putTile", &Game::putTile,
        "getTile", &Game::getTileptr
    );
}


