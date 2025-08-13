#include "Chunk.hpp"
#include "Variables.hpp"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string>
#include <json.hpp>

#include "PerlinNoise.hpp"

Chunk::Chunk(int x, int y) {
    //std::cout << "Loading chunk with " << std::this_thread::get_id() << std::endl;
    posX = x;
    posY = y;      
    if(!load()) {
        generate();
    };
    
    //std::cout << "Loaded chunk with " << std::this_thread::get_id() << std::endl;
}

bool Chunk::load() {
    using nlohmann::json;
    std::string path = SAVEPATH + "chunks/" + std::to_string(posX) + ":" + std::to_string(posY);
    std::ifstream save(path);
    if (save.fail()) return false;
    if (save.eof()) return false;

    json data = json::parse(save);
    if(data.empty()) return false;

    int x = 0;
    int y = 0;
    for (auto tiledata : data["tiles"]) {
        if (y == CHUNKSIZE) std::cerr << "Too many tiles in save " << path << "\n";
        Tile *tile = getRelativeTileptr(x, y);
        tile->setTypeNoLua(tiledata["id"]);
        tile->setPos(posX*CHUNKSIZE+x, posY*CHUNKSIZE+y);
        x++;
        if (x == CHUNKSIZE) {
            x = 0;
            y++;
        }
    }
    y = 0;
    x = 0;
    for (auto floordata : data["floors"]) {
        if (y == CHUNKSIZE) std::cerr << "Too many floors in save " << path << "\n";
        Floor *floor = getRelativeFloorptr(x, y);
        floor->setTypeNoLua(floordata["id"]);
        floor->setPos(posX*CHUNKSIZE+x, posY*CHUNKSIZE+y);
        x++;
        if (x == CHUNKSIZE) {
            x = 0;
            y++;
        }
    }
    return true;
};

void Chunk::generate() {
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlinNoise{seed};
    const double SCALE = 0.1;
    const double TREE_SCALE = 5;

    const float FIRST = 0.3;
    const float SECOND = 0.8;

    for (int y = 0; y < CHUNKSIZE; y++) {
        for (int x = 0; x < CHUNKSIZE; x++) {
            Floor *floor = getRelativeFloorptr(x, y);
            Vector2 pos {
                    (float)posX*CHUNKSIZE+x,
                    (float)posY*CHUNKSIZE+y
            };
            floor->setPos(pos.x, pos.y);
            double noise = perlinNoise.octave2D_01(pos.x*SCALE, pos.y*SCALE, 4);
            if (noise < FIRST) {
                floor->setTypeNoLua("water");
            } else if(noise < SECOND) {
                floor->setTypeNoLua("grass");
            } else {
                floor->setTypeNoLua("error");
            }
            double treeNoise = perlinNoise.octave2D_01(pos.x*TREE_SCALE, pos.y*TREE_SCALE, 4);
            if (treeNoise > 0.8) {
                Tile* tile = getRelativeTileptr(x,y);
                tile->setPos(x,y);
                tile->setTypeNoLua("tree");
            }
        }
    }
}

bool Chunk::save() {
    using nlohmann::json;
    std::string path = SAVEPATH + "chunks/" + std::to_string(posX) + ":" + std::to_string(posY);
    std::ofstream save(path);
    if (save.fail()) return false;

    json chunk;

    auto tilesdata = chunk["tiles"];

    for (auto &tile : tiles) {
        json tiledata = {{"id", tile.getType()->id}};
        tilesdata.push_back(tiledata);
    }
    chunk["tiles"] = tilesdata;
    auto floorsdata = chunk["floors"];

    for (auto &floor : floors) {
        json floordata = {{"id", floor.getType()->id}};
        floorsdata.push_back(floordata);
    }
    chunk["floors"] = floorsdata;
    save << chunk << std::endl;

    return true;
}

Tile *Chunk::getRelativeTileptr(uint x, uint y) {
    if(x >= CHUNKSIZE || y >= CHUNKSIZE) {
        return nullptr;
    }
    return tiles + y*CHUNKSIZE + x;
}

Tile *Chunk::getTileptr(Vector2 pos) {
    int x = std::floor(pos.x);
    int y = std::floor(pos.y);

    x = (x%CHUNKSIZE+CHUNKSIZE)%CHUNKSIZE;
    y = (y%CHUNKSIZE+CHUNKSIZE)%CHUNKSIZE;

    return tiles + y*CHUNKSIZE + x;
}

Floor *Chunk::getRelativeFloorptr(uint x, uint y) {
    if(x >= CHUNKSIZE || y >= CHUNKSIZE) {
        return nullptr;
    }
    return floors + y*CHUNKSIZE + x;
}

Floor *Chunk::getFloorptr(Vector2 pos) {
    int x = std::floor(pos.x);
    int y = std::floor(pos.y);

    x = (x%CHUNKSIZE+CHUNKSIZE)%CHUNKSIZE;
    y = (y%CHUNKSIZE+CHUNKSIZE)%CHUNKSIZE;

    return floors + y*CHUNKSIZE + x;
}


void Chunk::draw() {
    for (auto &floor : floors) {
        floor.draw();
    }
    for (auto &tile : tiles) {
        tile.draw();
    }
}

void Chunk::initialize() {
    for (int y = 0; y < CHUNKSIZE; y++) {
        for (int x = 0; x < CHUNKSIZE; x++) {
            Tile *tile = getRelativeTileptr(x, y);
            tile->setPos(posX*CHUNKSIZE+x, posY*CHUNKSIZE+y);
            tile->initialize();
            Floor *floor = getRelativeFloorptr(x, y);
            floor->setPos(posX*CHUNKSIZE+x, posY*CHUNKSIZE+y);
            floor->initialize();
        }
    }
}