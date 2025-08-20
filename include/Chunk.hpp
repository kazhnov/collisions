#include "Tile.hpp"
#include "Floor.hpp"

class Chunk {
    int posX, posY;
public:
    Tile tiles[CHUNKSIZE*CHUNKSIZE];
    Floor floors[CHUNKSIZE*CHUNKSIZE];
    Chunk(int x, int y);

    [[nodiscard]] Vector2 getPos() const {
        return {(float)posX, (float)posY};
    }

    void generate();

    void initialize();

    void drawTiles();
    void drawFloors();

    void setPos(Vector2 pos) {
        posX = std::floor(pos.x);
        posY = std::floor(pos.y);
    }

    Tile *getRelativeTileptr(uint x, uint y);
    Tile *getTileptr(Vector2 pos);

    Floor *getRelativeFloorptr(uint x, uint y);
    Floor *getFloorptr(Vector2 pos);

    bool load();

    bool save();

};