#include "Entity.hpp"
#include "Collider.hpp"
#include "Variables.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>

EntityType::EntityType(std::string id, Vector2 size, float speed):
    id(id), size(size), speed(speed)
{}

Entity::Entity(std::string id, Vector2 pos):
    type(EntityTypes::get(id)), 
    collider(pos, type->size),
    texture(LoadTexture((Variables::TexturePath+id+".png").c_str()))
{}

void Entity::setGoal(Vector2I pos) {
    goal = pos;
}

Vector2I Entity::getGoal() {
    return goal;
}

void Entity::moveToGoal() {
    if (route.empty()) return;
    vel = Vector2Normalize(Vector2Subtract({(float)(route.begin())->x, (float)route.begin()->y}, getPos()));
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

Vector2I::Vector2I(Vector2 vec) {
    this->x = std::floor(vec.x);
    this->y = std::floor(vec.y);
}

Vector2I getNeighbour(Vector2I pos, uint dir) {
    switch (dir) {
        case 0:
            pos.x += 1;
        case 1:
            pos.x -= 1;
        case 2: 
            pos.y += 1;
        case 3:
            pos.y -= 1;
    }
    return pos;
}

void sortNodes(std::vector<Node*> &nodes) {
    std::sort(nodes.begin(), nodes.end(), [](Node *a, Node *b) { 
        return a->sum() < b->sum();
    });
}

std::vector<Node*>::iterator getSame(std::vector<Node*> &nodes, Vector2I pos) {
    return std::find_if(nodes.begin(), nodes.end(), [pos](Node *node) {
        return node->pos.x == pos.x && node->pos.y == pos.y;
    });
}

void Entity::calculateRoute() {
    std::cout << "Calculating route" <<std::endl;
    float maxDistance = 8;
    if (!Variables::game->getTileptr({(float)getGoal().x, (float)getGoal().y})->getType()->isWalkable ||
        !Variables::game->getTileptr(getPos())->getType()->isWalkable) return;

    //std::vector<Node*> close{};
    std::vector<Node*> open{};
    std::vector<Node*> close{};
    open.push_back(new Node{Vector2I(getPos()), 0, getDistance(getPos(), goal), nullptr});
    
    Node *origin = open.back();
    Node *goalNode = nullptr;

    while(!open.empty() && goalNode == nullptr) {
        sortNodes(open);
        std::cout << open.size() << std::endl;
        Node *node = open.back();
        open.pop_back();

        if (getDistance(node->pos, goal) <= 1.) {
            std::cout <<"found" << std::endl;
            goalNode = node;
            continue;
        } else if (getDistance(node->pos, goal) > maxDistance) {
            close.push_back(node);
            continue;
        }

        for (int i = 0; i < 4; i++) {
            if(getSame(close, getNeighbour(node->pos, i)) != close.end()) {
                continue;
            }
            Node *neighbour = new Node;
            neighbour->pos = getNeighbour(node->pos, i);
            neighbour->parent = node;
            neighbour->to = node->to+1;
            neighbour->from = getDistance(neighbour->pos, goal);
            

            auto inOpen = getSame(open, neighbour->pos);
            if (inOpen != open.end()){
                if (open[inOpen - open.begin()]->sum() > neighbour->sum()) {
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
    std::cout << "Route found" <<std::endl;

    route.clear();

    while(goalNode != origin) {
        route.push_back(goalNode->pos);
        goalNode = goalNode->parent;
    }

    for (auto node : open) {
        delete node;
    }

    std::reverse(route.begin(), route.end());

}

void Entity::drawRoute() {
    for (auto &node : close) {
        Rectangle a;
        a.x = node->pos.x*Variables::PixelsPerMeter;
        a.y = node->pos.y*Variables::PixelsPerMeter;
        a.width = Variables::PixelsPerMeter;
        a.height = Variables::PixelsPerMeter;

        DrawRectangleRec(a, {255, 255, 0, 128});
    }

    Vector2 firstPos = Vector2Scale(getPos(), Variables::PixelsPerMeter);
    Vector2 secondPos;
    for (auto &pos : route) {
        secondPos = Vector2Scale(Vector2Add({(float)pos.x, (float)pos.y}, {0.5, 0.5}), Variables::PixelsPerMeter);
        DrawLineV(firstPos, secondPos, WHITE);
        firstPos = secondPos;
    }

}