#pragma once
#include "Collider.hpp"
#include "Variables.hpp"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <cute_c2.h>
#include <sol/forward.hpp>
#include <sol/property.hpp>
#include <sol/raii.hpp>
#include <sol/resolve.hpp>
#include <sol/variadic_args.hpp>
#include <string>
#include <vector>

class TileType {
public:
    const Texture2D texture;
    const Vector2 size;
    const Color color;
    const bool isWalkable;
    bool isScriptable = false;
    const std::string id;
    TileType(std::string id, Vector2 size, Color color, bool isWalkable) 
    : id(id), size(Vector2Clamp(size, {}, {1.f, 1.f})), color(color), texture(LoadTexture(("textures/" + id + ".png").c_str())),
    isWalkable(isWalkable)
    {
        if (!Variables::lua["TileScripts"].valid()) 
            Variables::lua["TileScripts"] = Variables::lua.create_table();
        if (!Variables::lua["TileScripts"][id].valid()) 
            Variables::lua["TileScripts"][id] = Variables::lua.create_table();
        std::cout << "Loaded tile \"" << id << "\"\n";
    }

    TileType(std::string id, Vector2 size) :
        TileType(id, size, WHITE, false)
    {
    }
};
class TileTypes {
private:    
    static inline std::vector<TileType> data = {};
public:
    static void add(std::string id, Vector2 size, Color color) {
        data.push_back(TileType(id, size, color, true));
    }
    static void add(std::string id, Vector2 size, Color color, bool isWalkable) {
        data.push_back(TileType(id, size, color, isWalkable));
    }

    static TileType *get(std::string id) {
        for (auto &tile : data) {
            if (tile.id == id) {
                return &tile;
            }
        }
        return get("error");
    }

    static void initLua() {
        Variables::lua.new_usertype<TileType>("TileType",
            "size", sol::readonly_property(&TileType::size),
            "id", sol::readonly_property(&TileType::id),
            "color", sol::readonly_property(&TileType::color),
            "isWalkable", sol::readonly_property(&TileType::isWalkable)
        );

        Variables::lua.set_function("addTileType", sol::resolve<void(std::string, Vector2, Color, bool)>(&TileTypes::add));
    }
};

class Tile {
private:
    int x;
    int y;
    TileType *type;
    Collider hitbox;
    sol::table data;
public:
    Tile(std::string id, Vector2 pos) :
        type(TileTypes::get(id)), hitbox({(int)(pos.x) + 0.5f, (int)(pos.y) + 0.5f}, type->size),
        x(std::floor(pos.x)), y(std::floor(pos.y))
    {
        data = Variables::lua.create_table();
    }

    ~Tile() {
        data = {};
    }

    Tile(): Tile("void", {0,0}) {}

    void draw();

    void setType(std::string id);

    Vector2 getPos() {
        return Vector2{(float)x, (float)y};
    }

    void setPos(int x, int y) {
        this->x = x;
        this->y = y;
        hitbox.setPos({(float)x + 0.5f, (float)y + 0.5f});
    }

    void setHitbox(Collider n) {
        hitbox = n;
    }

    Collider &getHitbox() {
        return hitbox;
    }

    TileType *getType() {
        return type;
    }

    static void initLua() {
        Variables::lua.new_usertype<Tile>("Tile",
        "type", sol::property(&Tile::getType, &Tile::setType),
        "pos", sol::property(&Tile::getPos, &Tile::setPos),
        "hitbox", sol::readonly_property(&Tile::getHitbox),
        "data", &Tile::data
    );
    }

};