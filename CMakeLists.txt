cmake_minimum_required(VERSION 3.6)


project(collisions)

include_directories(include/)
link_directories(/usr/local/lib/)
link_libraries(raylib lua lua++)
add_compile_options(-Wall -Wextra)
add_executable(collisions src/main.cpp src/Chunk.cpp src/Collider.cpp src/Display.cpp src/NPC.cpp src/Game.cpp src/Item.cpp src/Player.cpp src/Tile.cpp
        src/TileInfo.cpp
        src/Entity.cpp
        include/Placeable.hpp
        include/Floor.hpp
        src/Floor.cpp
)

