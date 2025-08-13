#pragma once
#include <vector>
#include <raylib.h>
#include <string>
#include <memory>

class Chunk;
class Tile;
class Floor;
class Display;
class Player;

class Game {
    std::vector<std::unique_ptr<Chunk>> chunks;
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
        void updateChunks();
        ~Game();

        void draw(); 
        static void initLua();

        void save();
};
