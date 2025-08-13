#include "Tile.hpp"
#include "Variables.hpp"
#include <mutex>
#include <string>
#include <iostream>
#include <utility>
#include "Player.hpp"
#include "Entity.hpp"
#include "NPC.hpp"

TileType::TileType(std::string id, Vector2 size, Color color, bool isWalkable) 
: id(id), size(Vector2Clamp(size, {}, {1.f, 1.f})), color(color), texture(LoadTexture((Variables::TexturePath + id + ".png").c_str())),
isWalkable(isWalkable)
{
    if (!Variables::lua["TileScripts"].valid()) 
        Variables::lua["TileScripts"] = Variables::lua.create_table();
    if (!Variables::lua["TileScripts"][id].valid()) 
        Variables::lua["TileScripts"][id] = Variables::lua.create_table();
    std::cout << "Loaded tile \"" << id << "\"\n";
}

TileType::TileType(std::string id, Vector2 size) :
    TileType(std::move(id), size, WHITE, false)
{
}

//Tile
Tile::Tile(const std::string& id, Vector2 pos) :
    x(std::floor(pos.x)), y(std::floor(pos.y)),
    type(TileTypes::get(id)), hitbox({float(pos.x) + 0.5f, float(pos.y) + 0.5f}, type->size)
{
    data = Variables::lua.create_table();
}

Tile::~Tile() {
    data = {};
}

Tile::Tile(): 
    x(0),
    y(0),
    type(TileTypes::get("void")), hitbox({}, {})
{

}

void Tile::draw() {
    if (type->texture.width) {
        Vector2 position = {
        hitbox.pos.x - hitbox.dim.x/2.f, hitbox.pos.y - hitbox.dim.y/2.f
        };
        position = Vector2Scale(position, Variables::PixelsPerMeter);
        float scale = (float)Variables::PixelsPerMeter/type->texture.width;
        DrawTextureEx(type->texture, position, 0, scale, type->color);
    }
    else {
        //Vector2 size = Vector2Scale(type.size, 0.9*Variables::PixelsPerMeter);
        //DrawRectangleV(position, size, type.color);
        hitbox.draw(type->color);
    }
}

void Tile::setType(std::string id) {
    type = TileTypes::get(id);
    hitbox.setDim({type->size.x, type->size.y});
    initialize();
}

void Tile::initialize() {
    data = Variables::lua.create_table();
    if (Variables::lua["TileScripts"][type->id].valid()){
        onCreate();
    }
}

void Tile::setTypeNoLua(const std::string& id) {
    type = TileTypes::get(id);
    hitbox.setDim({type->size.x, type->size.y});
}

Vector2 Tile::getPos() const {
    return Vector2{(float)x, (float)y};
}

void Tile::setPos(int x, int y) {
    this->x = x;
    this->y = y;
    hitbox.setPos({(float)x + 0.5f, (float)y + 0.5f});
}

void Tile::setHitbox(Collider n) {
    hitbox = n;
}

Collider &Tile::getHitbox() {
    return hitbox;
}

TileType *Tile::getType() {
    return type;
}

Tile::Tile(Tile &tile): hitbox(tile.hitbox) {
    this->type = tile.type;
    this->x = tile.x;
    this->y = tile.y;
    this->data = tile.data;

}

void Tile::onStandingNPC(NPC *player) {
    Variables::lua["TileScripts"][type->id]["onStanding"](this, player);
}

void Tile::onEnterNPC(NPC *player) {
    Variables::lua["TileScripts"][type->id]["onEnter"](this, player);
}

void Tile::onLeaveNPC(NPC *player){
    Variables::lua["TileScripts"][type->id]["onLeave"](this, player);
}

void Tile::onStandingPlayer(Player *player) {
    Variables::lua["TileScripts"][type->id]["onStanding"](this, player);
}

void Tile::onEnterPlayer(Player *player) {
    Variables::lua["TileScripts"][type->id]["onEnter"](this, player);
}

void Tile::onLeavePlayer(Player *player){
    Variables::lua["TileScripts"][type->id]["onLeave"](this, player);
}

void Tile::onStanding(Entity *entity) {
    if(auto *player = dynamic_cast<Player*>(entity)) {
        onStandingPlayer(player);
    } else {
        onStandingNPC((NPC*)entity);
    }
}
void Tile::onEnter(Entity *entity) {
    if(auto *player = dynamic_cast<Player*>(entity)) {
        //std::cout << "Player" << std::endl;
        onEnterPlayer(player);
    } else {
        //std::cout << "NPC" << std::endl;
        onEnterNPC((NPC*)entity);
    }
}
void Tile::onLeave(Entity *entity) {
    if(auto *player = dynamic_cast<Player*>(entity)) {
        onLeavePlayer(player);
    } else {
        onLeaveNPC((NPC*)entity);
    }
}
void Tile::onBreak() {
    Variables::lua["TileScripts"][type->id]["onBreak"](this);
}
void Tile::onCreate() {
    Variables::lua["TileScripts"][type->id]["onCreate"](this);
}
