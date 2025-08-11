#include "Entity.hpp"
#include "Variables.hpp"
#include "Tile.hpp"
#include <algorithm>
#include <cmath>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Game.hpp"
#include "TileInfo.hpp"

EntityType::EntityType(std::string id, Vector2 size, float speed):
    id(id), size(size), speed(speed)
{}

Entity::Entity(std::string id, Vector2 pos):
    type(EntityTypes::get(id)), 
    collider(pos, type->size),
    texture(LoadTexture((Variables::TexturePath+id+".png").c_str()))
{}

void Entity::setGoal(Vector2 pos) {
    goal = pos;
}

Vector2 Entity::getGoal() {
    return goal;
}

void Entity::moveToGoal(double delta) {
    if (route.empty()) return;
    vel = Vector2Normalize(Vector2Subtract(*route.begin(), getPos()));
    vel = Vector2Scale(vel, type->speed);
}

Vector2 Entity::getPos() {
    return collider.pos;
}

void Entity::setPos(Vector2 pos) {
    collider.pos = pos;
}

float getDistance(Vector2I pos, Vector2I goal) {
    //return Vector2Distance(pos, goal);
    return std::abs(pos.x - goal.x) + std::abs(pos.y - goal.y);
}

float Node::sum() {
    return to + from;
}

Node::Node(Vector2 pos, float to, float from, Node* parent) : pos(pos), to(to), from(from), parent(parent) {
}
Node::Node(): Node({}, 0, 0, nullptr){};

Vector2I::Vector2I(Vector2 vec) {
    this->x = std::floor(vec.x);
    this->y = std::floor(vec.y);
}

Vector2 getNeighbour(Vector2 pos, uint dir) {
    pos.x = std::floor(pos.x) + 0.5;
    pos.y = std::floor(pos.y) + 0.5;
    switch (dir) {
        case 0:
            pos.x += 1;
            break;
        case 1:
            pos.x -= 1;
            break;
        case 2: 
            pos.y += 1;
            break;
        case 3:
            pos.y -= 1;
            break;
        default:
            break;
    }
    return pos;
}

void sortNodes(std::vector<Node*> &nodes) {
    std::sort(nodes.begin(), nodes.end(), [](Node *a, Node *b) { 
        return a->sum() > b->sum();
    });
}

std::vector<Node*>::iterator getSame(std::vector<Node*> &nodes, Vector2 pos) {
    return std::find_if(nodes.begin(), nodes.end(), [pos](Node *node) {
        return std::floor(node->pos.x) == std::floor(pos.x) &&
             std::floor(node->pos.y) == std::floor(pos.y);
    });
}
void Entity::draw() {
    Vector2 position = {
            collider.pos.x - collider.dim.x/2.f, collider.pos.y - collider.dim.y/2.f
    };
    position = Vector2Scale(position, Variables::PixelsPerMeter);
    float scale = (float)Variables::PixelsPerMeter*collider.dim.x/texture.width;
    DrawTextureEx(texture, position, 0, scale, WHITE);
}

bool Entity::moveAndCollide(double delta) {
    double dt = delta/MAX_COLLISION_COUNT;

    std::vector<Tile*> tilesToCheck{};
    float posX = collider.pos.x;
    float posY = collider.pos.y;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            Tile *tile = Variables::game->getTileptr({posX+i,posY+j});
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
                    vel = Vector2Add(vel, Vector2Scale(collider.lastCollisionNormal, -Vector2DotProduct(collider.lastCollisionNormal, vel)));
                }
            }
        }
        if (i != MAX_COLLISION_COUNT)
            setPos(Vector2Add(getPos(), Vector2Scale(vel, dt)));

        for (auto tile : colliding) {
            if (std::find_if(prevColliding.begin(), prevColliding.end(), [tile](TileInfo &info){
                return info.tile == tile;
            }) == prevColliding.end()) {
            }
        }

        for (auto &tile : prevColliding) {
            auto found = std::find(colliding.begin(), colliding.end(), tile.tile);
            if (found == colliding.end() || colliding[found - colliding.begin()]->getType()->id != tile.id) {
                //Variables::lua["TileScripts"][tile.id]["onLeave"](tile.tiledata, this);
            } else {
                //Variables::lua["TileScripts"][tile.id]["onStanding"](tile.tiledata, this);
            }
        }

        prevColliding.clear();
        for (auto tile : colliding) {
            prevColliding.push_back(TileInfo(tile));
        }

        colliding.clear();
    }
    return false;
}

void Entity::calculateRoute() {
    float maxDistance = 16;
    if (!Variables::game->getTileptr(getGoal())->getType()->isWalkable ||
        !Variables::game->getTileptr(getPos())->getType()->isWalkable ||
            getDistance(getPos(), goal) < 0.1
        ) return;
    for (auto closeN : close) {
        delete closeN;
    }
    close.clear();
    route.clear();

    //std::vector<Node*> close{};
    std::vector<Node*> open{};
    //std::cout << getDistance(getPos(), goal) << std::endl;
    open.push_back(new Node(getPos(), 0, getDistance(getPos(), goal), nullptr));
    
    Node *origin = open.back();
    Node *goalNode = nullptr;

    while(!open.empty() && goalNode == nullptr) {
        sortNodes(open);
        Node *node = open.back();
        open.pop_back();
        if(getSame(close, node->pos)!=close.end()) {
            delete node;
            continue;
        }

        if (getDistance(node->pos, goal) <= 0.51) {
            goalNode = node;
            continue;
        } else if (node->sum() > maxDistance || !Variables::game->getTileptr(node->pos)->getType()->isWalkable) {
            close.push_back(node);
            continue;
        }

        for (int i = 0; i < 4; i++) {
            if(getSame(close, getNeighbour(node->pos, i)) != close.end()) {
                continue;
            }
            Node *neighbour = new Node{};
            neighbour->pos = getNeighbour(node->pos, i);
            neighbour->parent = node;
            neighbour->to = getDistance(neighbour->pos, node->pos);
            neighbour->from = getDistance(neighbour->pos, goal);
            

            auto inOpen = getSame(open, neighbour->pos);
            if (inOpen != open.end()){
                if (open[inOpen - open.begin()]->sum() <= neighbour->sum()) {
                    delete neighbour;
                } else {
                    Node *old = open[inOpen-open.begin()];
                    delete old;
                    open.erase(inOpen);
                    open.push_back(neighbour);
                }
            }
            else {
                open.push_back(neighbour);
            }
        }
    }
    if (goalNode == nullptr) return;
    route.clear();


    while(goalNode != origin) {
        route.push_back(goalNode->pos);
        goalNode = goalNode->parent;
    }

    for (auto node : open) {
        delete node;
    }

    delete goalNode;

    std::reverse(route.begin(), route.end());

}

void Entity::drawRoute() {
    /*
    for (auto &node : close) {
        Rectangle a;
        a.x = node->pos.x*Variables::PixelsPerMeter;
        a.y = node->pos.y*Variables::PixelsPerMeter;
        a.width = Variables::PixelsPerMeter;
        a.height = Variables::PixelsPerMeter;

        DrawRectangleRec(a, {255, 255, 0, 128});
    }
     */

    Vector2 firstPos = Vector2Scale(getPos(), Variables::PixelsPerMeter);
    Vector2 secondPos;
    for (auto &pos : route) {
        secondPos = Vector2Scale(Vector2Add({(float)pos.x, (float)pos.y}, {0.5, 0.5}), Variables::PixelsPerMeter);
        DrawLineV(firstPos, secondPos, WHITE);
        firstPos = secondPos;
    }

}