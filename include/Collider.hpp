#pragma once
#include <raylib.h>
#include <cute_c2.h>

class Collider {
public:
    Vector2 pos{};
    Vector2 dim{};
    bool collided = false;
    Vector2 lastCollisionNormal;
    Collider(Vector2 pos, Vector2 dimensions): pos(pos), dim(dimensions) {};

    c2AABB getAABB(); 

    void draw(Color color); 

    void drawOutline(Color color);

    bool preventCollisionWithStatic(Collider &target); 

    bool isColliding(Collider &target); 

    Vector2 getDim() {return dim;};

    void setDim(Vector2 dim) {this->dim = dim;};

    Vector2 getPos() {return pos;};

    void setPos(Vector2 pos) {this->pos = pos;};
};