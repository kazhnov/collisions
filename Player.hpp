#pragma once
#include <raylib.h>
#include <raymath.h>
#include <sol/sol.hpp>
#include "Tile.hpp"
#include "Collider.hpp"


class Game;

struct TileInfo {
    Tile *tile;
    Tile tiledata;
    std::string id;
};
struct Player {
    public:
        Collider collider;
        Game *game;
        Vector2 vel{0, 0};
        Color color;
        Texture texture;
        bool isOnFloor;
        float reach = 4.0;
        std::vector<TileInfo> prevColliding {};
        Tile *selectedTile;
        Player(Vector2 pos, Vector2 size, Color color): collider(pos, size), color(color) {
            texture = LoadTexture("textures/player.png");
        };

        Player() : collider(Vector2Zero(), {1.f, 1.f}), color(WHITE) {}

        void setColor(Color color) {
            this->color = color;
        }

        Color getColor() {
            return color;
        }

        static void initLua();

        void setGame(Game *game);

        bool moveAndCollideWithTiles(double delta); 

        bool moveAndCollideWithGame(double delta); 

        void applyAcceleration(Vector2 acc, double delta); 

        void accelerateTowards(Vector2 newVel, double delta, double scale); 

        void drawReach();

        Vector2 getPos(); 

        void putTile(Vector2 pos, std::string id);

        void putTileWithReach(Vector2 pos, std::string id, float reach);

        void setPos(Vector2 pos); 

        void draw(); 

        void sayHi();

        int getR() {
            return color.r;
        }

        void setR(int r) {
            color.r = r;
        }

        void drawCollisions();
};

