#include "Item.hpp"
#include "Tile.hpp"
#include "Player.hpp"
#include "Variables.hpp"
#include <iostream>
#include <raylib.h>
#include <sched.h>
#include <sstream>

//ItemType
ItemType::ItemType(std::string id, ItemEnum type)
    : id(id), texture(LoadTexture(("textures/" + id + ".png").c_str())), type(type){
        if (!Variables::lua["ItemScripts"].valid()) 
            Variables::lua["ItemScripts"] = Variables::lua.create_table();
        if (!Variables::lua["ItemScripts"][id].valid()) 
            Variables::lua["ItemScripts"][id] = Variables::lua.create_table();
        std::cout << "Loaded item \"" << id << "\"" << std::endl;
}

// Item
Item::Item(std::string id, uint count) {
    setType(id);
    this->count = count;
    this->data = Variables::lua.create_table();
}

ItemType* Item::getType() {
    return this->type;
}

void Item::setType(std::string id) {
    this->type = ItemTypes::get(id);
}

void Item::draw(Vector2 screenPos, Vector2 size, Vector2 anchor) {
    ItemType *type = getType();
    screenPos.x -= size.x*anchor.x;
    screenPos.y -= size.y*anchor.y;
    float scale = size.x/type->texture.width;
    DrawTextureEx(type->texture, screenPos, 0, scale, WHITE);
    std::stringstream s;
    s << count;
    float outlineSize = 1;
    float posX = screenPos.x;
    float posY = screenPos.y;
    const char *text = s.str().c_str();
    float fontSize = size.y/3;
    Color outlineColor = BLACK;
    Color color = WHITE;
    DrawText(text, posX - outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX - outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX, posY, fontSize, color);
}

void Item::use(Player *player, Vector2 pos) {
    if (type->type == ItemEnum::Block) {
        if (player->putTile(pos, type->id)){
            this->count--;
        }
    }
    Variables::lua["ItemScripts"][type->id]["onUse"](this, player, pos);
}