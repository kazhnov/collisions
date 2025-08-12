#pragma once
#include <string>
#include <memory>
#include "Tile.hpp"


struct TileInfo {
    Tile *tile;
    std::string id;
    Tile old;
    explicit TileInfo(Tile *tile);
    ~TileInfo();

};

