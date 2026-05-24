CFLAGS := $(shell pkg-config --cflags sdl3)
LDFLAGS := $(shell pkg-config --libs sdl3)

game: game.o world.o player.o camera.o render.o
	$(CC) -o $@ $^ $(LDFLAGS)

game.o: game.c world.h
	$(CC) $(CFLAGS) -c -o $@ $<

world.o: world.c world.h
	$(CC) $(CFLAGS) -c -o $@ $<

player.o: player.c world.h
	$(CC) $(CFLAGS) -c -o $@ $<

camera.o: camera.c camera.h
	$(CC) $(CFLAGS) -c -o $@ $<

render.o: render.c render.h world.h camera.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o game
