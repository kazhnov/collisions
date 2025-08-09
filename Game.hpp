#pragma once
#include "Player.hpp"
#include "Tile.hpp"
#include <future>
#include <iostream>
#include <mutex>
#include "Display.hpp"
#include "Variables.hpp"
#include "Chunk.hpp"

class Game {
    Tile *tiles;
    std::vector<Chunk> chunks{};
    static Chunk loadChunk(int x, int y);
    public:
        Display &display;
        Player &player;
        Texture texturePlayer;
        Game(Player &player, Display &display) : player(player), display(display) {
            this->player = player;
            std::cout << "world created" << std::endl;
            std::cout << "world initialized" << std::endl;
            Variables::lua["game"] = this;
        }
        Tile *getTileptr(Vector2 pos); 
        Chunk *getChunkptr(int x, int y);
        Chunk *getChunkptrFromPos(Vector2 pos);
        bool putTile(Vector2 pos, std::string id);
        void updateChunks();

        void draw(); 
        static void initLua();

        void save();
};
