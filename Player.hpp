#pragma once
#include <array>
#include <optional>
#include <raylib.h>
#include <raymath.h>
#include <sol/sol.hpp>
#include "Item.hpp"
#include "Tile.hpp"
#include "Collider.hpp"
#include "TileInfo.hpp"


class Game;
struct Player {
    public:
        Collider collider;
        Game *game;
        Vector2 vel{0, 0};
        Color color;
        Texture texture;
        bool isOnFloor;
        float reach = 4.0;
        //std::vector<Item> inventory{};
        std::array<std::optional<Item>, 36> inventory{};
        uint selectedSlot = 0;
        std::vector<TileInfo> prevColliding {};
        float cooldown;
        Player(Vector2 pos, Vector2 size, Color color): collider(pos, size), color(color) {
            texture = LoadTexture((Variables::TexturePath+"player.png").c_str());
        };

        Player() : collider(Vector2Zero(), {1.f, 1.f}), color(WHITE) {}

        void selectNext();

        void setColor(Color color) {
            this->color = color;
        }

        Color getColor() {
            return color;
        }

        static void initLua();

        void setGame(Game *game);

        void useSelected(Vector2 pos);

        bool moveAndCollideWithTiles(double delta); 

        bool moveAndCollideWithGame(double delta); 

        void applyAcceleration(Vector2 acc, double delta); 

        void accelerateTowards(Vector2 newVel, double delta, double scale); 

        void drawReach();

        Vector2 getPos(); 

        bool putTile(Vector2 pos, std::string id);

        bool putTileWithReach(Vector2 pos, std::string id, float reach);

        void setPos(Vector2 pos); 

        void draw(); 

        int getR() {
            return color.r;
        }

        void setR(int r) {
            color.r = r;
        }

        void drawCollisions();
};

