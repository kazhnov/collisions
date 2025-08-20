#pragma once
#include <vector>
#include <raylib.h>
#include <string>
#include <memory>
#include "Item.hpp"
#include "Entity.hpp"
#include "ItemEntity.hpp"
#include "NPC.hpp"

class Chunk;
class Tile;
class Floor;
class Display;
class Player;

class Game {
    std::vector<std::unique_ptr<Chunk>> chunks;
    std::vector<Entity*> entities;
    static Chunk* loadChunk(int x, int y);
    public:
        Display &display;
        Player &player;
        Texture texturePlayer;
        Game(Player &player, Display &display);
        Tile *getTileptr(Vector2 pos);
        Floor *getFloorptr(Vector2 pos);
        Chunk *getChunkptr(int x, int y);
        Chunk *getChunkptrFromPos(Vector2 pos);
        bool putTile(Vector2 pos, std::string id);
        std::optional<Item> breakTile(Vector2 pos);
        void updateChunks();
        void addItemEntity(Item entity, Vector2 pos);
        void addNPC(NPC entity);
        std::vector<Entity*> getEntityFromPos(Vector2 pos, float radius);
        std::vector<Entity*>& getEntities();
        ~Game();

        void update(double delta);

        void draw(); 
        static void initLua();

        void save();
};
