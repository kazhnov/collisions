#include "Tile.hpp"
#include "Variables.hpp"
#include <raylib.h>

class Chunk {
    int posX, posY;
public:
    Tile tiles[CHUNKSIZE*CHUNKSIZE];
    Chunk(int x, int y); 

    [[nodiscard]] Vector2 getPos() const {
        return {(float)posX, (float)posY};
    }

    void generate();

    void initialize();

    void draw();

    void setPos(Vector2 pos) {
        posX = std::floor(pos.x);
        posY = std::floor(pos.y);
    }

    Tile *getRelativeTileptr(uint x, uint y);
    Tile *getTileptr(Vector2 pos);

    bool load();

    bool save();

};