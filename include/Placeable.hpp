#pragma once
#include <string>
#include <raylib.h>
#include "Collider.hpp"

class Entity;
class Player;
class NPC;

class Placeable {
public:

    virtual void setType(std::string id) = 0;

    virtual void setTypeNoLua(const std::string& id) = 0;

    [[nodiscard]] virtual Vector2 getPos() const = 0;

    virtual void setPos(int x, int y) = 0;

    virtual void setHitbox(Collider n) = 0;

    virtual Collider &getHitbox() = 0;

    virtual void initialize() = 0;

    virtual void onStanding(Entity *player) = 0;
    virtual void onEnter(Entity *player) = 0;
    virtual void onLeave(Entity *player) = 0;

    virtual void onStandingNPC(NPC *player) = 0;
    virtual void onEnterNPC(NPC *player) = 0;
    virtual void onLeaveNPC(NPC *player) = 0;

    virtual void onStandingPlayer(Player *player) = 0;
    virtual void onEnterPlayer(Player *player) = 0;
    virtual void onLeavePlayer(Player *player) = 0;

    virtual void onBreak() = 0;
    virtual void onCreate() = 0;
};
