#include "Tile.hpp"
#include "Variables.hpp"
#include <string>
#include "Player.hpp"

TileType::TileType(std::string id, Vector2 size, Color color, bool isWalkable) 
: id(id), size(Vector2Clamp(size, {}, {1.f, 1.f})), color(color), texture(LoadTexture(("textures/" + id + ".png").c_str())),
isWalkable(isWalkable)
{
    if (!Variables::lua["TileScripts"].valid()) 
        Variables::lua["TileScripts"] = Variables::lua.create_table();
    if (!Variables::lua["TileScripts"][id].valid()) 
        Variables::lua["TileScripts"][id] = Variables::lua.create_table();
    std::cout << "Loaded tile \"" << id << "\"\n";
}

TileType::TileType(std::string id, Vector2 size) :
    TileType(id, size, WHITE, false)
{
}

//Tile
Tile::Tile(std::string id, Vector2 pos) :
    type(TileTypes::get(id)), hitbox({(int)(pos.x) + 0.5f, (int)(pos.y) + 0.5f}, type->size),
    x(std::floor(pos.x)), y(std::floor(pos.y))
{
    data = Variables::lua.create_table();
}

Tile::~Tile() {
    data = {};
}

Tile::Tile(): Tile("void", {0,0}) {}


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
    data = Variables::lua.create_table();
    Variables::lua["TileScripts"][id]["onCreate"](this);
}

Vector2 Tile::getPos() {
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

void Tile::onStanding(Player *player) {
    Variables::lua["TileScripts"][type->id]["onStanding"](this, player);
}

void Tile::onEnter(Player *player) {
    Variables::lua["TileScripts"][type->id]["onEnter"](this, player);
}

void Tile::onLeave(Player *player){
    Variables::lua["TileScripts"][type->id]["onLeave"](this, player);
}