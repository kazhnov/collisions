#pragma once
#include <raylib.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <sol/table.hpp>
#include <string>
#include "Variables.hpp"

enum class ItemEnum {
    Block,
    Other
};

class ItemType {
public:
    const ItemEnum type;
    const std::string id;
    const Texture2D texture;
    const uint stack;

    ItemType(std::string id, ItemEnum type, uint stack);
    ItemType(std::string id, ItemEnum type);
};

class ItemTypes {
public:
    static inline std::vector<ItemType> data{};
    static void add(const std::string& id, ItemEnum type) {
        data.emplace_back(id, type);
    }

    static ItemType *get(const std::string& id) {
        for (auto &item : data) {
            if (item.id == id) {
                return &item;
            }
        }
        return get("error");
    }

    static void initLua() {
        Variables::lua.new_enum<ItemEnum>("ItemEnum",
            {
                {"Block", ItemEnum::Block},
                {"Other", ItemEnum::Other}

            }
        );
        Variables::lua.new_usertype<ItemType>("ItemType",
            "id", sol::readonly_property(&ItemType::id)
        );

        Variables::lua.set_function("addItemType", &ItemTypes::add);
    }
};

class Player;

struct Item {
private:
    ItemType *type;
public:
    uint count;
    sol::table data;

    Item(std::string id, uint count);

    ItemType* getType();

    void setType(std::string id);

    void draw(Vector2 pos, Vector2 size, Vector2 anchor);

    static void initLua() {
        Variables::lua.new_usertype<Item>("Item",
            "count", &Item::count,
            "type", sol::property(&Item::getType, &Item::setType),
            "data", &Item::data,
            "draw", &Item::draw
        );
    }

    void use(Player *player, Vector2 pos);
};