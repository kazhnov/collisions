build/main.o: main.cpp
	g++ main.cpp -o build/main.o -c

build/Player.o: Player.cpp
	g++ Player.cpp -o build/Player.o -c

build/Game.o: Game.cpp
	g++ Game.cpp -o build/Game.o -c

build/Tile.o: Tile.cpp
	g++ Tile.cpp -o build/Tile.o -c

build/Display.o: Display.cpp
	g++ Display.cpp -o build/Display.o -c

build/Collider.o: Collider.cpp
	g++ Collider.cpp -o build/Collider.o -c

build/Chunk.o: Chunk.cpp
	g++ Chunk.cpp -o build/Chunk.o -c

build/Collisions: build/main.o build/Player.o build/Game.o build/Display.o build/Collider.o build/Tile.o build/Chunk.o *.hpp
	g++ build/main.o build/Player.o build/Game.o build/Display.o build/Collider.o build/Tile.o build/Chunk.o -o build/Collisions -lraylib -llua++ -llua

all: build/Collisions
	build/Collisions