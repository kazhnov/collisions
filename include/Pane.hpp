#pragma once
#include <array>
#include <cstddef>
#include <optional>
#include <raylib.h>
#include <sys/types.h>
#include <vector>
#include "Item.hpp"
namespace GUI {

    template<int amount>
    struct Pane {
        std::array<std::optional<Item>, amount> &items;
        uint &selected;
        uint columns;
        Vector2 pos, size, anchor;
        bool visible;
        static inline float thickness = 10;
        static inline float itemThickness = 5;
        Pane(std::array<std::optional<Item>, amount> &items, uint &selected, uint columns) 
        :items(items), columns(columns), selected(selected)
        {
        }

        void draw() {
            Vector2 position;
            position.x = pos.x - size.x * anchor.x;
            position.y = pos.y - size.y * anchor.y;
            Rectangle frame = Rectangle{.x = position.x, .y = position.y, .width = size.x, .height = size.y};
            DrawRectangleRec(frame, GRAY);
            frame.x += thickness;
            frame.y += thickness;
            frame.width  -= thickness*2;
            frame.height -= thickness*2;
            DrawRectangleRec(frame, LIGHTGRAY);
            float itemsize = frame.width/columns;
            int i = 0, j = 0;
            for (int k = 0; k < items.size(); k++) {
                Vector2 itempos;
                itempos.x = frame.x + itemsize*j;
                itempos.y = frame.y + itemsize*i;
                drawItem(k, itempos, {itemsize, itemsize}, {0, 0});
                j++;
                if (j == columns) {
                    i++;
                    j = 0;
                }
            }
        }

        void drawItem(uint i, Vector2 pos, Vector2 size, Vector2 anchor) {
            Color bg = LIGHTGRAY;
            if(get(GetMousePosition())==i) bg = WHITE;
            Vector2 position;
            position.x = pos.x - size.x * anchor.x;
            position.y = pos.y - size.y * anchor.y;
            Rectangle frame = Rectangle{.x = position.x, .y = position.y, .width = size.x, .height = size.y};
            DrawRectangleRec(frame, i==selected? YELLOW : GRAY);
            frame.x += itemThickness;
            frame.y += itemThickness;
            frame.width  -= itemThickness*2;
            frame.height -= itemThickness*2;
            DrawRectangleRec(frame, bg);
            if (items.at(i).has_value())
                items.at(i)->draw({frame.x, frame.y}, {frame.width, frame.height}, {});
        };

        uint get(Vector2 mouse) {
            Vector2 position;
            position.x = pos.x - size.x * anchor.x + thickness;
            position.y = pos.y - size.y * anchor.y + thickness;
            if (mouse.x < position.x || mouse.y < position.y || 
                mouse.x > position.x + size.x - thickness||
                mouse.y > position.y + size.y - thickness
            ) {
                return -1;
            }
            float itemsize = (size.x - thickness*2)/columns;
            mouse.x -= position.x;
            mouse.y -= position.y;
            uint x = mouse.x/itemsize;
            uint y = mouse.y/itemsize;
            if (y*columns + x >= items.size()) return -1;
            return y*columns+x;
        }

    };

}