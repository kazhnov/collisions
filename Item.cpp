#include "Item.hpp"
#include "Tile.hpp"
#include "Player.hpp"
#include "Variables.hpp"
#include <iostream>
#include <raylib.h>

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
}

void Item::use(Player *player, Vector2 pos) {
    if (type->type == ItemEnum::Block) {
        player->putTile(pos, type->id);
    }
    Variables::lua["ItemScripts"][type->id]["onUse"](this, player, pos);
}