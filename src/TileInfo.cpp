#include "TileInfo.hpp"
#include "Tile.hpp"

TileInfo::TileInfo(Tile *tile) {
    this->tile = tile;
    this->id = tile->getType()->id;
    this->old = Tile(tile->getType()->id, tile->getPos());
    this->old.data = tile->data;
}

TileInfo::~TileInfo() = default;

