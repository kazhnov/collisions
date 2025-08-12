#include "Collider.hpp"
#include "Variables.hpp"
#define CUTE_C2_IMPLEMENTATION
#include <cute_c2.h>
#include <raymath.h>
    
void Collider::draw(Color color) {
    if (dim.x * dim.y == 0) return;
    Vector2 position = {
    pos.x - dim.x/2.f, pos.y - dim.y/2.f
    };
    position = Vector2Scale(position, Variables::PixelsPerMeter);
    DrawRectangle(position.x, position.y, dim.x * Variables::PixelsPerMeter, dim.y*Variables::PixelsPerMeter, color);
}

void Collider::drawOutline(Color color) {
    Vector2 position = {
    pos.x - dim.x/2.f, pos.y - dim.y/2.f
    };
    position = Vector2Scale(position, Variables::PixelsPerMeter);
    DrawRectangleLines(position.x, position.y, dim.x * Variables::PixelsPerMeter, dim.y*Variables::PixelsPerMeter, color);
}

c2AABB Collider::getAABB() {
    return c2AABB{
        .min = {pos.x - dim.x/2.f, pos.y - dim.y/2.f},
        .max = {pos.x + dim.x/2.f, pos.y + dim.y/2.f},
    };
}

bool Collider::preventCollisionWithStatic(Collider &target) {
    c2Manifold manifold;
    c2AABBtoAABBManifold(target.getAABB(), getAABB(), &manifold);

    collided = isColliding(target);

    pos.x += manifold.n.x * manifold.depths[0] * 1.015;
    pos.y += manifold.n.y * manifold.depths[0] * 1.015;
    if (collided) {
        lastCollisionNormal = {manifold.n.x, manifold.n.y};
    }
    return collided;
}

bool Collider::isColliding(Collider &target) {
    return c2AABBtoAABB(target.getAABB(), getAABB());
}