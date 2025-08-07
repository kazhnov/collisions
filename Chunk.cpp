#include "Chunk.hpp"
#include "Variables.hpp"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string>
#include <json.hpp>

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
        tile->setType(tiledata["id"]);
        tile->setPos(posX*CHUNKSIZE+x, posY*CHUNKSIZE+y);
        x++;
        if (x == CHUNKSIZE) {
            x = 0;
            y++;
        }
    }
    return true;
};

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
    pos.x += 0.5f;
    pos.y += 0.5f;
    int x = std::floor(pos.x);
    int y = std::floor(pos.y);

    x = (x%CHUNKSIZE+CHUNKSIZE)%CHUNKSIZE;
    y = (y%CHUNKSIZE+CHUNKSIZE)%CHUNKSIZE;

    return tiles + y*CHUNKSIZE + x;
}

void Chunk::draw() {
    for (auto &tile : tiles) {
        tile.draw();
    }/*
    DrawRectangleLines(
        posX*CHUNKSIZE*Variables::PixelsPerMeter, 
        posY*CHUNKSIZE*Variables::PixelsPerMeter, 
        CHUNKSIZE*Variables::PixelsPerMeter, 
        CHUNKSIZE*Variables::PixelsPerMeter, 
        YELLOW);
        */
}

void Chunk::initialize() {
    for (int y = 0; y < CHUNKSIZE; y++) {
        for (int x = 0; x < CHUNKSIZE; x++) {
            getRelativeTileptr(x, y)->setPos(posX*CHUNKSIZE+x, posY*CHUNKSIZE+y);
        }
    }
}