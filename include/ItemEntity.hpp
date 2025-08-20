#pragma once
#include "Entity.hpp"
#include "Item.hpp"

class ItemEntity : public Entity {
private:
    Collider collider;
    Vector2 vel;
public:
    Item item;
    ItemEntity(Item item, Vector2 pos);
    Vector2 getPos() override;
    void setPos(Vector2 pos) override;

    Vector2 getVel() override;
    void setVel(Vector2 vel) override;

    Collider* getCollider() override;
    void setCollider(Collider collider) override;

    void accelerateTowards(Vector2 newVel, double delta, double scale);
    void draw() override;

    ~ItemEntity() override;

    void onEntityCollision(Entity *entity);
};