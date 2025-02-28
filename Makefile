all:
c	g++ -Iinclude -Llib -o bin/main.exe src/main.c src/traffic_simulation.c -lmingw32 -lSDL2main -lSDL2