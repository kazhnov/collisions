#pragma once
#include <cmath>
#include <optional>
#include <sol/forward.hpp>
#include <sol/object_base.hpp>
#include <sys/types.h>
#include <sol/sol.hpp>
#include <raylib.h>
const int MAX_COLLISION_COUNT = 16;
const float G = 2000;
const bool DEBUGINFO = false;
const uint CHUNKSIZE = 16;
const std::string SAVEPATH = std::string("saves/demo/");

enum TileScript {
    onBlock
};

class Variables {
public:
    static inline uint PixelsPerMeter = 64;
    static inline sol::state lua;
    static std::optional<sol::function> getScript(std::string id, TileScript type) {
            auto script = lua["TileScripts"][id];
            if (script.valid()) return std::nullopt;
            switch (type) {
            case onBlock:
                return script["onBlock"];
            }
        }
    static inline int RenderDistance = 3;
};

Vector2 Vector2Floor(Vector2 v);
