#include "Entity.hpp"
#define CUTE_C2_IMPLEMENTATION
#include <cute_c2.h>
#define SOL_ALL_SAFETIES_ON 1
#include "gui/Pane.hpp"
#include "Tile.hpp"
#include "Item.hpp"
#include "rayliblua.hpp"
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "Game.hpp"
#include "Player.hpp"
#include "Display.hpp"
#include "Variables.hpp"
#include <sol/sol.hpp>
#include <filesystem>

Vector2 Vector2Floor(Vector2 v) {
    return Vector2{std::floor(v.x), std::floor(v.y)};
}

int main() {
    Display display;
    display.width = 1280;
    display.height = 800;
    InitWindow(display.width,display.height, "Penis");

    Variables::lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
    setupLuaBindings(Variables::lua);
    Player::initLua();
    Game::initLua();
    Tile::initLua();
    TileTypes::initLua();
    Item::initLua();
    ItemTypes::initLua();
    EntityTypes::initLua();
    Variables::lua.do_file("../lua/init.lua");
    Variables::RenderDistance = 4;

    for (auto &entry : std::filesystem::directory_iterator("../lua/tiles")) {
        Variables::lua.do_file(entry.path());
    }
    for (auto &entry : std::filesystem::directory_iterator("../lua/items")) {
        Variables::lua.do_file(entry.path());
    }
    for (auto &entry : std::filesystem::directory_iterator("../lua/entities")) {
        Variables::lua.do_file(entry.path());
    }

    SetTargetFPS(60);

    Camera2D camera;
    camera.zoom = 1.f;
    camera.rotation = 0;
    camera.offset = {display.width/2.f - Variables::PixelsPerMeter/2.f, display.height/2.f - Variables::PixelsPerMeter/2.f};

    Player player({1, 1}, {0.8f, 0.8f}, GREEN);
    Game game(player, display);
    Variables::game = &game;
    player.game = &game;
    game.player = player;
    GUI::Pane<36> inventory(player.inventory, player.selectedSlot, 9);
    inventory.anchor = {0.5f, 0.5f};
    inventory.visible = false;

    Variables::lua["player"] = &player;
    Variables::lua["game"] = &game;

    Color color;

    for (int i = 0; i < std::min((size_t)36, ItemTypes::data.size()); i++)
        player.inventory.at(i) = Item(ItemTypes::data.at(i).id, 64);

    /*
    Entity enemy("enemy", {});
    enemy.setGoal({3, 5});
    enemy.calculateRoute();
    */

    while (!WindowShouldClose()) {
        double delta = GetFrameTime();
        display.width = GetScreenWidth();
        display.height = GetScreenHeight();

        inventory.pos.x = display.width/2.;
        inventory.pos.y = display.height/2.;

        inventory.size.x = display.width*0.75;
        inventory.size.y = display.height*0.75;

        Variables::lua["update"](delta);

        Vector2 mouseScreen = GetMousePosition();
        Vector2 mouse = GetScreenToWorld2D(mouseScreen, camera);
        mouse = Vector2Scale(mouse, 1.f/Variables::PixelsPerMeter);
        mouse.x -= 0.5;
        mouse.y -= 0.5;
        
        if (IsKeyPressed(KEY_TAB)) {
            inventory.visible = !inventory.visible;
        }
        if (!inventory.visible) {
            Vector2 deltaVel = {};
            if (IsKeyDown(KEY_A)) {
                deltaVel.x -= 1;
            }
            if (IsKeyDown(KEY_D)) {
                deltaVel.x += 1;
            }
            if (IsKeyDown(KEY_W)) {
                deltaVel.y -= 1;
            }
            if (IsKeyDown(KEY_S)) {
                deltaVel.y += 1;
            }
            Variables::PixelsPerMeter += GetMouseWheelMove() * 5;
            Variables::PixelsPerMeter = Clamp(Variables::PixelsPerMeter, 16, 256);
            deltaVel = Vector2Normalize(deltaVel);
            if (deltaVel.x || deltaVel.y) {
                player.accelerateTowards(Vector2Scale(deltaVel, 10), delta, 2);
            }
            else {
                player.accelerateTowards({}, delta, 10);
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                player.useSelected(mouse);
            }
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                player.putTile(mouse, "void");
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                auto item = inventory.get(mouseScreen);
                if (item != -1)
                    player.selectedSlot = item;
            }
            player.accelerateTowards({}, delta, 10);
        }
        player.moveAndCollideWithTiles(delta);

        camera.target = Vector2Scale(player.getPos(), Variables::PixelsPerMeter);

        //enemy.calculateRoute();

        BeginDrawing(); {
            ClearBackground(BLACK);
            BeginMode2D(camera); {
                

                if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    player.drawReach();
                }

                //enemy.drawRoute();
                game.draw();
                player.draw();

                //player.drawCollisions();

                //player.selectedTile->draw();
            } EndMode2D();
            if (inventory.visible){
                inventory.draw();
            }
            std::stringstream s;
            s << GetFPS() << '\n' << mouse.x << "\t" << mouse.y;
            DrawText(s.str().c_str(), 32, 32, 32, RAYWHITE);

        } EndDrawing();
        game.updateChunks();
        player.cooldown -= delta;
        if (player.cooldown < 0) player.cooldown = 0;
    }

    game.save();
    CloseWindow();

    return 0;
}
