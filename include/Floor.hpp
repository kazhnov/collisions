#pragma once
#include <raylib.h>
#include <string>
#include <vector>
#include "Placeable.hpp"
#include "Variables.hpp"
#include <sol/sol.hpp>


class FloorType {
public:
    const Texture2D texture;
    const Vector2 size;
    const Color color;
    const bool isWalkable;
    const std::string id;

    FloorType(std::string id, Vector2 size, Color color, bool isWalkable);

    FloorType(std::string id, Vector2 size);
};

class FloorTypes {
private:
    static inline std::vector<FloorType> data = {};
public:
    static void add(const std::string& id, Vector2 size, Color color) {
        data.emplace_back(id, size, color, true);
    }
    static void add(const std::string& id, Vector2 size, Color color, bool isWalkable) {
        data.emplace_back(id, size, color, isWalkable);
    }

    static FloorType *get(const std::string& id) {
        for (auto &tile : data) {
            if (tile.id == id) {
                return &tile;
            }
        }
        return get("error");
    }

    static void initLua() {
        Variables::lua.new_usertype<FloorType>("FloorType",
                                              "size", sol::readonly_property(&FloorType::size),
                                              "id", sol::readonly_property(&FloorType::id),
                                              "color", sol::readonly_property(&FloorType::color),
                                              "isWalkable", sol::readonly_property(&FloorType::isWalkable)
        );

        Variables::lua.set_function("addFloorType", sol::resolve<void(const std::string&, Vector2, Color, bool)>(&FloorTypes::add));
    }
};
class Floor : Placeable{
private:
    int x;
    int y;
    FloorType *type;
    Collider hitbox;
public:
    sol::table data;

    Floor(const std::string& id, Vector2 pos);

    ~Floor();

    Floor(Floor &floor);

    Floor();

    void draw();
    void setType(std::string id) override;

    void setTypeNoLua(const std::string& id) override;

    [[nodiscard]] Vector2 getPos() const override;

    void setPos(int x, int y) override;

    void setHitbox(Collider n) override;

    Collider &getHitbox() override;

    void initialize() override;

    FloorType *getType();

    static void initLua() {
        Variables::lua.new_usertype<Floor>("Floor",
                                          "type", sol::property(&Floor::getType, &Floor::setType),
                                          "pos", sol::property(&Floor::getPos, &Floor::setPos),
                                          "hitbox", sol::readonly_property(&Floor::getHitbox),
                                          "data", &Floor::data
        );
    }

    void onStanding(Entity *player) override;
    void onEnter(Entity *player) override;
    void onLeave(Entity *player) override;

    void onStandingNPC(NPC *player) override;
    void onEnterNPC(NPC *player) override;
    void onLeaveNPC(NPC *player) override;

    void onStandingPlayer(Player *player) override;
    void onEnterPlayer(Player *player) override;
    void onLeavePlayer(Player *player) override;

    void onBreak() override;
    void onCreate() override;
};

