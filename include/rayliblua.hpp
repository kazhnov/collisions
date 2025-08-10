#pragma once
#include <raylib.h>
#include <sol/sol.hpp>

void setupLuaBindings(sol::state &lua) {
  lua.new_usertype<Color>("Color",
    sol::call_constructor, sol::factories(
      [](){
        return Color{};
      },
      [](float r, float g, float b) {
        return Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};
      },
      [](float r, float g, float b, float a) {
        return Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
      }
    ),
    "r", sol::property(
      [&](Color &self){return self.r;},
      [&](Color &self, float r){self.r = r;}
    ),
    "g", sol::property(
      [&](Color &self){return self.g;},
      [&](Color &self, float g){self.g = g;}
    ),
    "b", sol::property(
      [&](Color &self){return self.b;},
      [&](Color &self, float b){self.b = b;}
    ),
    "a", sol::property(
      [&](Color &self){return self.a;},
      [&](Color &self, float a){self.a = a;}
    ),

    sol::meta_function::to_string, [](Color& c) {
      return TextFormat("{r: %02.02f, g: %02.02f, b: %02.02f}", c.r, c.g, c.b);
    });

    lua.new_usertype<Vector2>("Vector2",
    sol::call_constructor, sol::factories(
      [](){
        return Vector2{};
      },
      [](float x, float y){
        return Vector2{x, y};
      }
    ),
		"x", &Vector2::x,
		"y", &Vector2::y,
    sol::meta_function::to_string, [](Vector2& v) {
      return TextFormat("{x: %02.02f, y: %02.02f}", v.x, v.y);
    });
}