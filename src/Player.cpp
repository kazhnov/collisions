#include "Player.hpp"
#include "Game.hpp"
#include "Tile.hpp"
#include "TileInfo.hpp"
#include "Variables.hpp"
#include <algorithm>
#include <optional>
#include <raylib.h>
#include <raymath.h>
#include <sol/raii.hpp>
#include <sol/sol.hpp>
#include <variant>
#include <vector>

Player::Player(Vector2 pos, Vector2 size, Color color): collider(pos, size), color(color) {
    texture = LoadTexture((Variables::TexturePath+"player.png").c_str());
};

Player::Player() : collider(Vector2Zero(), {1.f, 1.f}), color(WHITE) {}

void Player::applyAcceleration(Vector2 acc, double delta) {
    vel.x += acc.x * delta;
    vel.y += acc.y * delta;
}

void Player::accelerateTowards(Vector2 newVel, double delta, double scale) {
    vel = Vector2Lerp(vel, newVel, scale*delta);
}

Vector2 Player::getPos() {
    return collider.getPos();
}

void Player::setPos(Vector2 pos) {
    collider.setPos(pos);
}

void Player::setVel(Vector2 vel) {
    this->vel = vel;
}

Vector2 Player::getVel() {
    return vel;
}

void Player::draw() {
    Vector2 position = {
        collider.pos.x - collider.dim.x/2.f, collider.pos.y - collider.dim.y/2.f
    };
    position = Vector2Scale(position, Variables::PixelsPerMeter);
    float scale = (float)Variables::PixelsPerMeter*collider.dim.x/texture.width;
    DrawTextureEx(texture, position, 0, scale, WHITE);
}

void Player::drawReach() {
    Vector2 position = {
    getPos().x, getPos().y
    };
    position = Vector2Scale(position, Variables::PixelsPerMeter);

    DrawCircleLinesV(position, reach*Variables::PixelsPerMeter, reach > 0 ? WHITE : RED);
}

void Player::drawCollisions() {
    for (auto &tile : this->prevColliding) {
        tile->tile->getHitbox().drawOutline(RED);
    }
}

void Player::useSelected(Vector2 pos) {
    if (!cooldown) {
        if (inventory.at(selectedSlot).has_value()){
            inventory.at(selectedSlot).value().use(this, pos);
            cooldown = Variables::UseCoolDown;
            if (inventory.at(selectedSlot)->count <= 0) {
                inventory.at(selectedSlot) = std::nullopt;
            }
        }
    }
}

bool Player::putTile(Vector2 pos, std::string id) {
    return putTileWithReach(pos, id, reach);
}

bool Player::putTileWithReach(Vector2 pos, std::string id, float reach) {
    if(Vector2Distance(pos, getPos()) > reach) return false;
    return game->putTile(pos, id);
}


void Player::initLua() {
    Variables::lua.new_usertype<Player>("Player",
        "reach", &Player::reach,
        "pos", sol::property(&Player::getPos, &Player::setPos),
        "vel", sol::property(&Player::getVel, &Player::setVel),
        "applyAcceleration", &Player::applyAcceleration,
        "accelerateTowards", &Player::accelerateTowards,
        "putTile", &Player::putTile,
        "putTileWithReach", &Player::putTileWithReach
    );
}

Collider *Player::getCollider() {
    return &collider;
}

void Player::setCollider(Collider collider) {
    this->collider = collider;
}

bool Player::breakTile(Vector2 pos) {
    return breakTileWithReach(pos, reach);
}

bool Player::breakTileWithReach(Vector2 pos, float reach) {
    if(Vector2Distance(pos, getPos()) > reach) return false;
    auto item = Variables::game->breakTile(pos);

    if (item.has_value()) {
        Variables::game->addItemEntity(item.value(), pos);
        return true;
    }
    return false;
}

void Player::addItem(Item item) {
    for (auto &slot : inventory) {
        if (slot.has_value() && slot.value().getType()->id == item.getType()->id) {
            slot.value().count += item.count;
            return;
        }
        if (!slot.has_value()) {
            std::cout <<"found" <<std::endl;
            slot = item;
            return;
        }
    }
}

void Player::suckEntity(Entity *entity) {
    auto vec = Vector2Subtract(getPos(), entity->getPos());
    float length = Vector2Length(vec);
    vec = Vector2Normalize(vec);
    vec = Vector2Scale(vec, 1.f/(length*length));
    entity->setVel(Vector2Add(entity->getVel(), vec));
}

Player::~Player() = default;

void Player::onEntityCollision(Entity *entity) {

}

