#pragma once

#include <raylib.h>
#include <vector>

#include "Collider.hpp"
struct TileInfo;

class Entity {
    public:
        std::vector<TileInfo*> prevColliding{};

        virtual Vector2 getPos() = 0;
        virtual void setPos(Vector2 pos) = 0;

        virtual void setVel(Vector2 vel) = 0;
        virtual Vector2 getVel() = 0;

        virtual Collider *getCollider() = 0;
        virtual void setCollider(Collider collider) = 0;

        bool moveAndCollide(double delta);
};
