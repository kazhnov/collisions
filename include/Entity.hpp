#pragma once
#include <string>
#include <vector>
#include "Variables.hpp"
#include "Collider.hpp"

struct TileInfo;

struct EntityType {
    const std::string id;
    const Vector2 size;
    const float speed;
    EntityType(std::string id, Vector2 size, float speed);
};

struct EntityTypes {
    static inline std::vector<EntityType> data{};
    static void add(std::string id, Vector2 size, float speed) {
        data.push_back(EntityType(id, size, speed));
    }

    static EntityType *get(std::string id) {
        for (auto &entity : data) {
            if (entity.id == id) {
                return &entity;
            }
        }
        return get("error");
    }

    static void initLua() {
        Variables::lua.new_usertype<EntityType>("EntityType",
            "id", sol::readonly_property(&EntityType::id)
        );

        Variables::lua.set_function("addEntityType", &EntityTypes::add);
    }
};

struct Node {
    Vector2 pos;
    float to, from;
    Node *parent;
    Node(Vector2 pos, float to, float from, Node *parent);
    Node();
    float sum();
};

class Entity {
    std::vector<Node*> close{};
    EntityType *type;
    std::vector<Vector2> route{};
    Vector2 goal;
public:

    bool moveAndCollide(double delta);
    Collider collider;
    Vector2 vel{};
    Texture texture;
    std::vector<TileInfo*> prevColliding{};
    Entity(std::string id, Vector2 pos);
    void setGoal(Vector2 pos);
    Vector2 getGoal();
    Vector2 getPos();
    void setPos(Vector2 pos);
    void moveToGoal(double delta);
    void calculateRoute();
    void drawRoute();
    void draw();
    static void initLua();
};