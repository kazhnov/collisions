#include "Game.hpp"
#include "Variables.hpp"
#include <sol/forward.hpp>

int Game::getSizeX() {
    return sizeX;
}
int Game::getSizeY() {
    return sizeY;
}
void Game::initialize() {
    tiles = new Tile[sizeY * sizeX];
    for (uint y = 0; y < sizeY; y++) {
        for (uint x = 0; x < sizeX; x++) {
            Tile *tile = getTileptr(Vector2{(float)x, (float)y});
            tile->setType("void");
            tile->setPos(x, y);
        }
    }
}
Tile *Game::getTileptr(Vector2 pos) {
    pos.x += 0.5f;
    pos.y += 0.5f;
    uint x = pos.x;
    uint y = pos.y;
    if(x >= sizeX || y >= sizeY) {
        return nullptr;
    }
    return tiles + y*sizeX + x;
}

void Game::putTile(Vector2 pos, std::string id) {
    Tile *old = getTileptr(pos);
    if (old == nullptr) return;
    if (old->getType()->id == id) return;
    Tile tile = Tile(*old);
    tile.setType(id);
    if (tile.getHitbox().isColliding(player.collider)) return;
    Variables::lua["TileScripts"][old->getType()->id]["onDelete"](old);

    old->setType(id);
    Variables::lua["TileScripts"][id]["onCreate"](old);

}

void Game::draw() {
    for (uint y = 0; y < sizeY; y++) {
        for (uint x = 0; x < sizeX; x++) {
            Tile *tile = getTileptr(Vector2{(float)x, (float)y});
            tile->draw();
        }
    }
}

void Game::initLua() {
    Variables::lua.new_usertype<Game>("Game",
        "tiles", &Game::tiles,
        "putTile", &Game::putTile,
        "getTile", &Game::getTileptr
    );
}


