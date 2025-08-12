#include "NPC.hpp"
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

NPCType::NPCType(std::string id, Vector2 size, float speed):
    id(id), size(size), speed(speed)
{}

NPC::NPC(std::string id, Vector2 pos):
    type(NPCTypes::get(id)),
    collider(pos, type->size),
    texture(LoadTexture((Variables::TexturePath+id+".png").c_str()))
{}

void NPC::setGoal(Vector2 pos) {
    goal = {floor(pos.x)+0.5f, floor(pos.y)+0.5f};
}

Vector2 NPC::getGoal() {
    return goal;
}

void NPC::moveToGoal(double delta) {
    if (route.empty()) {
        vel = {};
        return;
    }
    vel = Vector2Subtract(*route.begin(), getPos());
    if (Vector2Length(vel) <= 0.1) return;
    vel = Vector2Normalize(vel);
    vel = Vector2Scale(vel, type->speed);
}

Vector2 NPC::getPos() {
    return collider.pos;
}

void NPC::setPos(Vector2 pos) {
    collider.pos = pos;
}

float getDistance(Vector2 pos, Vector2 goal) {
    //return Vector2Distance(pos, goal);
    return std::abs(pos.x - goal.x) + std::abs(pos.y - goal.y);
}

float Node::sum() {
    return to + from;
}

Node::Node(Vector2 pos, float to, float from, Node* parent) : pos(pos), to(to), from(from), parent(parent) {
}
Node::Node(): Node({}, 0, 0, nullptr){};


Vector2 getNeighbour(Vector2 pos, uint dir) {
    pos.x = std::floor(pos.x) + 0.5f;
    pos.y = std::floor(pos.y) + 0.5f;
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
void NPC::draw() {
    Vector2 position = {
            collider.pos.x - collider.dim.x/2.f, collider.pos.y - collider.dim.y/2.f
    };
    position = Vector2Scale(position, Variables::PixelsPerMeter);
    float scale = (float)Variables::PixelsPerMeter*collider.dim.x/texture.width;
    DrawTextureEx(texture, position, 0, scale, WHITE);
}

void NPC::calculateRoute() {
    float maxDistance = 16;
    float minDistance = 0.5;
    if (!Variables::game->getTileptr(getGoal())->getType()->isWalkable ||
        !Variables::game->getTileptr(getPos())->getType()->isWalkable) return;
    if (getDistance(getPos(), goal) <= minDistance) {
        route.clear();
        route.push_back(goal);
        return;
    }


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

        if (getDistance(node->pos, goal) <= minDistance) {
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
            neighbour->to = getDistance(neighbour->pos, node->pos) + node->to;
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
    route.push_back(goal);

    while (goalNode != origin) {
        route.push_back(goalNode->pos);
        goalNode = goalNode->parent;
    }

    for (auto node : open) {
        delete node;
    }

    delete goalNode;

    std::reverse(route.begin(), route.end());

}

void NPC::drawRoute() {
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
        secondPos = Vector2Scale(pos, Variables::PixelsPerMeter);
        DrawLineV(firstPos, secondPos, WHITE);
        firstPos = secondPos;
    }

}

void NPC::initLua() {
    Variables::lua.new_usertype<NPC>(
            "NPC",
            "goal", sol::property(&NPC::getGoal, &NPC::setGoal),
            "vel", sol::property(&NPC::getVel, &NPC::setVel),
            "pos", sol::property(&NPC::getPos, &NPC::setPos)
            );

}

Collider *NPC::getCollider() {
    return &collider;
}

void NPC::setCollider(Collider collider) {
    this->collider = collider;
}

void NPC::setVel(Vector2 vel) {
    this->vel = vel;
}

Vector2 NPC::getVel() {
    return vel;
}
