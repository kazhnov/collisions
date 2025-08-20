#pragma once
#include <array>
#include <optional>
#include <raylib.h>
#include <raymath.h>
#include "Item.hpp"
#include "Collider.hpp"
#include "Entity.hpp"

struct TileInfo;

class Game;
class Player : public Entity {
    public:
        Collider collider;
        Game *game;
        Vector2 vel{0, 0};
        Color color;
        Texture texture;
        float reach = 4.0;
        std::array<std::optional<Item>, 36> inventory{};
        uint selectedSlot = 0;
        float cooldown;
        Player(Vector2 pos, Vector2 size, Color color);

        Player();

        static void initLua();

        void useSelected(Vector2 pos);

        void applyAcceleration(Vector2 acc, double delta);

        void accelerateTowards(Vector2 newVel, double delta, double scale); 

        void drawReach();

        bool putTile(Vector2 pos, std::string id);

        bool putTileWithReach(Vector2 pos, std::string id, float reach);

        bool breakTile(Vector2 pos);

        bool breakTileWithReach(Vector2 pos, float reach);

        void draw() override;

        void drawCollisions();

        void addItem(Item item);

        Vector2 getPos() override;

        void setPos(Vector2 pos) override;

        void setVel(Vector2 vel) override;

        Vector2 getVel() override;

        Collider *getCollider() override;

        void setCollider(Collider collider) override;

        ~Player() override;

        void suckEntity(Entity *entity);

    void onEntityCollision(Entity *entity);
};

