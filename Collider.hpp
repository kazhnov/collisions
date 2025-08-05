#pragma once
#include <raylib.h>
#include <cute_c2.h>

enum Shape {
    SHAPE_CIRCLE,
    SHAPE_BOX,
};

class Collider {
public:
    Vector2 dim;
    Vector2 pos;
    bool collided = false;
    Vector2 lastCollisionNormal;
    Collider(Vector2 pos, Vector2 dimensions): pos(pos), dim(dimensions) {};

    c2AABB getAABB(); 

    void draw(Color color); 

    bool preventCollisionWithStatic(Collider &target); 

    bool isColliding(Collider &target); 

    Vector2 getDim() {return dim;};

    void setDim(Vector2 dim) {this->dim = dim;};

    Vector2 getPos() {return pos;};

    void setPos(Vector2 pos) {this->pos = pos;};
};