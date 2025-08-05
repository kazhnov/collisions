#pragma once
#include "Player.hpp"
#include "Tile.hpp"
#include <iostream>
#include "Display.hpp"
#include "Variables.hpp"

class Game {
    Tile *tiles;
    const uint sizeX;
    const uint sizeY;
    public:
        Display &display;
        Player &player;
        Texture texturePlayer;
        Game(Player &player, Display &display, uint x, uint y) : player(player), display(display), sizeX(x), sizeY(y) {
            this->player = player;
            std::cout << "world created" << std::endl;
            initialize();
            std::cout << "world initialized" << std::endl;
            Variables::lua["game"] = this;
        }
        int getSizeX(); 
        int getSizeY(); 
        void initialize(); 
        Tile *getTileptr(Vector2 pos); 
        void putTile(Vector2 pos, std::string id);
        void draw(); 
        static void initLua();
};
