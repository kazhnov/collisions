#include <raylib.h>
#include <raymath.h>
#include "ItemEntity.hpp"

Vector2 ItemEntity::getPos() {
    return collider.pos;
}

void ItemEntity::setPos(Vector2 pos) {
    collider.pos = pos;
}

Collider *ItemEntity::getCollider() {
    return &collider;
}

void ItemEntity::setCollider(Collider collider) {
    this->collider = collider;
}

void ItemEntity::setVel(Vector2 vel) {
    this->vel = vel;
}

Vector2 ItemEntity::getVel() {
    return vel;
}

ItemEntity::ItemEntity(Item item, Vector2 pos) :
    collider(pos, {0.3, 0.3}), item(item.getType()->id, item.count),
    vel()
    {

}

void ItemEntity::accelerateTowards(Vector2 newVel, double delta, double scale) {
    vel = Vector2Lerp(vel, newVel, scale*delta);
}
void ItemEntity::draw() {
    item.draw(Vector2Scale(getPos(), Variables::PixelsPerMeter),
              Vector2Scale(getCollider()->getDim(), Variables::PixelsPerMeter), {0.5f, 0.5f}, false);
}

ItemEntity::~ItemEntity() = default;

void ItemEntity::onEntityCollision(Entity *entity) {}
