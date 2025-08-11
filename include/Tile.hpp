#pragma once
#include "Collider.hpp"
#include "Variables.hpp"
#include <raylib.h>
#include <raymath.h>
#include <cute_c2.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <string>
#include <vector>

struct TileType {
    const Texture2D texture;
    const Vector2 size;
    const Color color;
    const bool isWalkable;
    bool isScriptable = false;
    const std::string id;
    TileType(std::string id, Vector2 size, Color color, bool isWalkable);

    TileType(std::string id, Vector2 size);
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
class Player;
class Tile {
private:
    int x;
    int y;
    TileType *type;
    Collider hitbox;
public:
    sol::table data;
    Tile(std::string id, Vector2 pos);

    ~Tile();

    Tile(Tile &tile);

    Tile();

    void draw();

    void setType(std::string id);

    void setTypeNoLua(std::string id);

    Vector2 getPos();

    void setPos(int x, int y);

    void setHitbox(Collider n);

    Collider &getHitbox();

    TileType *getType(); 

    void initialize();

    static void initLua() {
        Variables::lua.new_usertype<Tile>("Tile",
        "type", sol::property(&Tile::getType, &Tile::setType),
        "pos", sol::property(&Tile::getPos, &Tile::setPos),
        "hitbox", sol::readonly_property(&Tile::getHitbox),
        "data", &Tile::data
    );
    }

    void onStanding(Player *player);
    void onEnter(Player *player);
    void onLeave(Player *player);
};