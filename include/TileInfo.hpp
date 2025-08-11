#pragma once
#include <string>
#include <memory>
#include "Tile.hpp"

class Tile;

struct TileInfo {
    Tile *tile;
    std::string id;
    Tile old;
    explicit TileInfo(TileInfo tile);
    ~TileInfo();

};

