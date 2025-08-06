#define CUTE_C2_IMPLEMENTATION
#include <cute_c2.h>
#define SOL_ALL_SAFETIES_ON 1
#include "Tile.hpp"
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
    Variables::lua.do_file("lua/init.lua");
    
    for (auto &entry : std::filesystem::directory_iterator("lua/tiles")) {
        Variables::lua.do_file(entry.path());
    }

    //SetTargetFPS(60);

    Camera2D camera;
    camera.zoom = 1.f;
    camera.rotation = 0;
    camera.offset = {display.width/2.f - Variables::PixelsPerMeter/2.f, display.height/2.f - Variables::PixelsPerMeter/2.f};

    Player player({1, 1}, {0.8f, 0.8f}, GREEN);
    Game game(player, display);
    player.selectedTile = game.getTileptr({});
    player.game = &game;
    game.player = player;

    Variables::lua["player"] = &player;
    Variables::lua["game"] = &game;

    Color color;

    while (!WindowShouldClose()) {
        double delta = GetFrameTime();

        Variables::lua["update"](delta);

        Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
        mouse = Vector2Scale(mouse, 1.f/Variables::PixelsPerMeter);
        mouse.x -= 0.5;
        mouse.y -= 0.5;
        
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
            player.putTile(mouse, "reach");
        }
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            player.putTile(mouse, "void");
        }
        player.moveAndCollideWithTiles(delta);

        camera.target = Vector2Scale(player.getPos(), Variables::PixelsPerMeter);
        

        BeginDrawing(); {
            ClearBackground(BLACK);
            BeginMode2D(camera); {
                
                game.draw();
                player.draw();

                if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    player.drawReach();
                }

                player.drawCollisions();

                //player.selectedTile->draw();
            } EndMode2D();
            std::stringstream s;
            s << GetFPS() << '\n' << mouse.x << "\t" << mouse.y;
            DrawText(s.str().c_str(), 32, 32, 32, RAYWHITE);

        } EndDrawing();
        game.updateChunks();
    }

    game.save();
    CloseWindow();

    return 0;
}
