cmake_minimum_required(VERSION 3.6)

project(collisions)

include_directories(include/)
link_directories(/usr/local/lib/)
link_libraries(raylib lua lua++)
add_executable(collisions src/main.cpp src/Chunk.cpp src/Collider.cpp src/Display.cpp src/Entity.cpp src/Game.cpp src/Item.cpp src/Player.cpp src/Tile.cpp)

