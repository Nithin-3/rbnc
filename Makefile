CFLAGS := $(shell pkg-config --cflags sdl3)
LDFLAGS := $(shell pkg-config --libs sdl3) -lm

game: game.o world.o player.o camera.o render.o object.o
	$(CC) -o $@ $^ $(LDFLAGS)

game.o: game.c world.h render.h player.h camera.h
	$(CC) $(CFLAGS) -c -o $@ $<

world.o: world.c world.h
	$(CC) $(CFLAGS) -c -o $@ $<

player.o: player.c player.h world.h
	$(CC) $(CFLAGS) -c -o $@ $<

camera.o: camera.c camera.h
	$(CC) $(CFLAGS) -c -o $@ $<

render.o: render.c render.h world.h camera.h
	$(CC) $(CFLAGS) -c -o $@ $<

object.o: object.c object.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o game
