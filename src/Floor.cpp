#include "Floor.hpp"
#include "Variables.hpp"
#include <mutex>
#include <string>
#include <iostream>
#include <utility>
#include "Player.hpp"
#include "Entity.hpp"
#include "NPC.hpp"

FloorType::FloorType(std::string id, Vector2 size, Color color, bool isWalkable)
: id(id), size(Vector2Clamp(size, {}, {1.f, 1.f})), color(color), texture(LoadTexture((Variables::TexturePath + id + ".png").c_str())),
isWalkable(isWalkable)
{
    if (!Variables::lua["FloorScripts"].valid())
        Variables::lua["FloorScripts"] = Variables::lua.create_table();
    if (!Variables::lua["FloorScripts"][id].valid())
        Variables::lua["FloorScripts"][id] = Variables::lua.create_table();
    std::cout << "Loaded floor \"" << id << "\"\n";
}

FloorType::FloorType(std::string id, Vector2 size) :
    FloorType(std::move(id), size, WHITE, false)
{
}

//Tile
Floor::Floor(const std::string& id, Vector2 pos) :
    x(std::floor(pos.x)), y(std::floor(pos.y)),
    type(FloorTypes::get(id)), hitbox({float(pos.x) + 0.5f, float(pos.y) + 0.5f}, type->size)
{
    data = Variables::lua.create_table();
}

Floor::~Floor() {
    data = {};
}

Floor::Floor():
    x(0),
    y(0),
    type(FloorTypes::get("water")), hitbox({}, {})
{

}

void Floor::draw() {
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

void Floor::setType(std::string id) {
    type = FloorTypes::get(id);
    hitbox.setDim({type->size.x, type->size.y});
    initialize();
}

void Floor::initialize() {
    data = Variables::lua.create_table();
    if (Variables::lua["FloorScripts"][type->id].valid()){
        onCreate();
    }
}

void Floor::setTypeNoLua(const std::string& id) {
    type = FloorTypes::get(id);
    hitbox.setDim({type->size.x, type->size.y});
}

Vector2 Floor::getPos() const {
    return Vector2{(float)x, (float)y};
}

void Floor::setPos(int x, int y) {
    this->x = x;
    this->y = y;
    hitbox.setPos({(float)x + 0.5f, (float)y + 0.5f});
}

void Floor::setHitbox(Collider n) {
    hitbox = n;
}

Collider &Floor::getHitbox() {
    return hitbox;
}

FloorType *Floor::getType() {
    return type;
}

Floor::Floor(Floor &floor): hitbox(floor.hitbox) {
    this->type = floor.type;
    this->x = floor.x;
    this->y = floor.y;
    this->data = floor.data;

}

void Floor::onStandingNPC(NPC *player) {
    Variables::lua["FloorScripts"][type->id]["onStanding"](this, player);
}

void Floor::onEnterNPC(NPC *player) {
    Variables::lua["FloorScripts"][type->id]["onEnter"](this, player);
}

void Floor::onLeaveNPC(NPC *player){
    Variables::lua["FloorScripts"][type->id]["onLeave"](this, player);
}

void Floor::onStandingPlayer(Player *player) {
    Variables::lua["FloorScripts"][type->id]["onStanding"](this, player);
}

void Floor::onEnterPlayer(Player *player) {
    Variables::lua["FloorScripts"][type->id]["onEnter"](this, player);
}

void Floor::onLeavePlayer(Player *player){
    Variables::lua["FloorScripts"][type->id]["onLeave"](this, player);
}

void Floor::onStanding(Entity *entity) {
    if(auto *player = dynamic_cast<Player*>(entity)) {
        onStandingPlayer(player);
    } else {
        onStandingNPC((NPC*)entity);
    }
}
void Floor::onEnter(Entity *entity) {
    if(auto *player = dynamic_cast<Player*>(entity)) {
        //std::cout << "Player" << std::endl;
        onEnterPlayer(player);
    } else {
        //std::cout << "NPC" << std::endl;
        onEnterNPC((NPC*)entity);
    }
}
void Floor::onLeave(Entity *entity) {
    if(auto *player = dynamic_cast<Player*>(entity)) {
        onLeavePlayer(player);
    } else {
        onLeaveNPC((NPC*)entity);
    }
}
void Floor::onBreak() {
    Variables::lua["FloorScripts"][type->id]["onBreak"](this);
}
void Floor::onCreate() {
    Variables::lua["FloorScripts"][type->id]["onCreate"](this);
}
