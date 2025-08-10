#pragma once
#include <array>
#include <optional>
#include <raylib.h>
#include <raymath.h>
#include "Item.hpp"
#include "TileInfo.hpp"
#include "Collider.hpp"

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
        Player(Vector2 pos, Vector2 size, Color color);

        Player();

        void selectNext();

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

        void drawCollisions();
};

