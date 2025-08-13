#include "Entity.hpp"
#include "Variables.hpp"
#include "Tile.hpp"
#include "Floor.hpp"
#include "TileInfo.hpp"
#include "Collider.hpp"
#include "Game.hpp"


bool Entity::moveAndCollide(double delta) {
    double dt = delta/MAX_COLLISION_COUNT;

    std::vector<Tile*> tilesToCheck{};
    std::vector<Floor*> floorsToCheck{};
    float posX = getCollider()->pos.x;
    float posY = getCollider()->pos.y;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            Tile *tile = Variables::game->getTileptr({posX+i,posY+j});
            tilesToCheck.push_back(tile);
            Floor *floor = Variables::game->getFloorptr({posX+i,posY+j});
            floorsToCheck.push_back(floor);
        }
    }

    std::vector<Tile*> colliding{};
    for (int i = 0; i < MAX_COLLISION_COUNT + 1; i++) {
        for (int j = 0; j < tilesToCheck.size(); j++) {
            Tile* tile = tilesToCheck.at(j);
            Collider collision = tile->getHitbox();
            if (collision.getDim().x*collision.getDim().y > 0 && getCollider()->isColliding(collision)) {
                colliding.push_back(tile);
                if (!tile->getType()->isWalkable) {
                    getCollider()->preventCollisionWithStatic(collision);
                    float collisionFactor = -Vector2DotProduct(getCollider()->lastCollisionNormal, getVel());
                    collisionFactor = std::max(collisionFactor, 0.f);
                    setVel(Vector2Add(getVel(), Vector2Scale(getCollider()->lastCollisionNormal, collisionFactor)));
                }
            }
            Floor* floor = floorsToCheck.at(j);
            collision = floor->getHitbox();
            if (getCollider()->isColliding(collision)) {
                if (!floor->getType()->isWalkable) {
                    getCollider()->preventCollisionWithStatic(collision);
                    float collisionFactor = -Vector2DotProduct(getCollider()->lastCollisionNormal, getVel());
                    collisionFactor = std::max(collisionFactor, 0.f);
                    setVel(Vector2Add(getVel(), Vector2Scale(getCollider()->lastCollisionNormal, collisionFactor)));
                }
            }
        }
        if (i != MAX_COLLISION_COUNT)
            setPos(Vector2Add(getPos(), Vector2Scale(getVel(), dt)));

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