#pragma once
#include <string>
#include <vector>
#include "Variables.hpp"
#include "Collider.hpp"
#include "Entity.hpp"

struct TileInfo;

struct NPCType {
    const std::string id;
    const Vector2 size;
    const float speed;
    NPCType(std::string id, Vector2 size, float speed);
};

struct NPCTypes {
    static inline std::vector<NPCType> data{};
    static void add(const std::string& id, Vector2 size, float speed) {
        data.emplace_back(id, size, speed);
    }

    static NPCType *get(const std::string& id) {
        for (auto &entity : data) {
            if (entity.id == id) {
                return &entity;
            }
        }
        return get("error");
    }

    static void initLua() {
        Variables::lua.new_usertype<NPCType>("NPCType",
                                             "id", sol::readonly_property(&NPCType::id)
        );

        Variables::lua.set_function("addNPCType", &NPCTypes::add);
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

class NPC : public Entity {
    std::vector<Node*> close{};
    NPCType *type;
    std::vector<Vector2> route{};
    Vector2 goal;
public:

    Collider collider;
    Vector2 vel{};
    Texture texture;
    NPC(std::string id, Vector2 pos);

    void setGoal(Vector2 pos);
    Vector2 getGoal();

    Vector2 getPos() override;
    void setPos(Vector2 pos) override;

    Vector2 getVel() override;
    void setVel(Vector2 vel) override;

    Collider* getCollider() override;
    void setCollider(Collider collider) override;

    void moveToGoal(double delta);
    void calculateRoute();
    void drawRoute();
    void draw();
    static void initLua();
};