#include "Tile.hpp"
#include "Variables.hpp"

void Tile::draw() {
    if (type->texture.width) {
        Vector2 position = {
        hitbox.pos.x - hitbox.dim.x/2.f, hitbox.pos.y - hitbox.dim.y/2.f
        };
        position = Vector2Scale(position, Variables::PixelsPerMeter);
        float scale = (float)Variables::PixelsPerMeter/type->texture.width;
        DrawTextureEx(type->texture, position, 0, scale, type->color);
    }
    else {
        //Vector2 size = Vector2Scale(type.size, 0.9*Variables::PixelsPerMeter);
        //DrawRectangleV(position, size, type.color);
        hitbox.draw(type->color);
    }
}

void Tile::setType(std::string id) {
    type = TileTypes::get(id);
    hitbox.setDim({type->size.x, type->size.y});
    data = Variables::lua.create_table();
    Variables::lua["TileScripts"][id]["onCreate"](this);
}
