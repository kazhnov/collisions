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
void Player::setGame(Game *game) {
    this->game = game;
}

bool Player::moveAndCollideWithTiles(double delta) {
    double dt = delta/MAX_COLLISION_COUNT;

    std::vector<Tile*> tilesToCheck{};
    float posX = collider.pos.x;
    float posY = collider.pos.y;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            Tile *tile = game->getTileptr({posX+i,posY+j});
            if (tile->getHitbox().getDim().x * tile->getHitbox().getDim().y == 0) continue;
            tilesToCheck.push_back(tile);
        }
    }

    std::vector<Tile*> colliding{};
    for (int i = 0; i < MAX_COLLISION_COUNT + 1; i++) {
        for (auto tile : tilesToCheck) {
            Collider collision = tile->getHitbox();
            if (collider.isColliding(collision)) {
                colliding.push_back(tile);
                if (!tile->getType()->isWalkable) {
                    collider.preventCollisionWithStatic(collision);
                    float collisionFactor = -Vector2DotProduct(collider.lastCollisionNormal, vel);
                    collisionFactor = std::max(collisionFactor, 0.f);
                    vel = Vector2Add(vel, Vector2Scale(collider.lastCollisionNormal, collisionFactor));
                }
            }
        }
        if (i != MAX_COLLISION_COUNT)
            setPos(Vector2Add(getPos(), Vector2Scale(vel, dt)));

        for (auto tile : colliding) {
            if (std::find_if(prevColliding.begin(), prevColliding.end(), [tile](TileInfo *info){
                return info->tile == tile;
            }) == prevColliding.end()) {
                tile->onEnter(this);
            }
        }

        for (auto &tile : prevColliding) {
            auto found = std::find(colliding.begin(), colliding.end(), tile->tile);
            if (found == colliding.end() || colliding[found - colliding.begin()]->getType()->id != tile->id) {
                tile->old.onLeave(this);
            } else {
                tile->tile->onStanding(this);
            }
        }

        for (auto info : prevColliding) {
            delete info;
        }
        prevColliding.clear();
        for (auto &tile: colliding) {
            TileInfo *info = new TileInfo(tile);
            prevColliding.push_back(info);
        }


        colliding.clear();
    }
    return false;
}

void Player::selectNext() {
    selectedSlot++;
    if (selectedSlot == 3) {
        selectedSlot = 0;
    }
}

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
        "pos", sol::property(&Player::getPos, &Player::setPos),
        "vel", &Player::vel,
        "applyAcceleration", &Player::applyAcceleration,
        "accelerateTowards", &Player::accelerateTowards,
        "putTile", &Player::putTile,
        "putTileWithReach", &Player::putTileWithReach,
        "color", &Player::color,
        "reach", &Player::reach
    );
}

