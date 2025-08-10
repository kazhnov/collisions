cmake_minimum_required(VERSION 3.6)

project(collisions)

link_directories(/usr/local/lib/)
link_libraries(raylib lua lua++)
add_executable(collisions main.cpp Chunk.cpp Collider.cpp Display.cpp Entity.cpp Game.cpp Item.cpp Player.cpp Tile.cpp)