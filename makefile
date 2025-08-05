build/main.o: main.cpp rayliblua.hpp Variables.hpp
	g++ main.cpp -o build/main.o -c

build/Player.o: Player.cpp Player.hpp
	g++ Player.cpp -o build/Player.o -c

build/Game.o: Game.cpp Game.hpp
	g++ Game.cpp -o build/Game.o -c

build/Tile.o: Tile.cpp Tile.hpp
	g++ Tile.cpp -o build/Tile.o -c

build/Display.o: Display.cpp Display.hpp
	g++ Display.cpp -o build/Display.o -c

build/Collider.o: Collider.cpp Collider.hpp
	g++ Collider.cpp -o build/Collider.o -c

build/Collisions: build/main.o build/Player.o build/Game.o build/Display.o build/Collider.o
	g++ build/main.o build/Player.o build/Game.o build/Display.o build/Collider.o -o build/Collisions -lraylib -llua++ -llua

all: build/Collisions
	build/Collisions