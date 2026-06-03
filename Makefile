CFLAGS := $(shell pkg-config --cflags sdl3)
LDFLAGS := $(shell pkg-config --libs sdl3) -lm $(shell pkg-config --libs libcurl)
OBJDIR := out

game: $(OBJDIR)/game.o $(OBJDIR)/world.o $(OBJDIR)/player.o \
      $(OBJDIR)/camera.o $(OBJDIR)/render.o $(OBJDIR)/object.o \
      $(OBJDIR)/args.o $(OBJDIR)/ws.o $(OBJDIR)/hud.o
	$(CC) -o $(OBJDIR)/$@ $^ $(LDFLAGS)

$(OBJDIR)/game.o: game.c world.h render.h player.h camera.h ws.h hud.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/world.o: world.c world.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/player.o: player.c player.h world.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/camera.o: camera.c camera.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/render.o: render.c render.h world.h camera.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/object.o: object.c object.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/args.o: args.c args.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/ws.o: ws.c ws.h args.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/hud.o: hud.c hud.h world.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

format:
	find -name "*.[ch]" | xargs clang-format -i

check-format:
	find -name "*.[ch]" | xargs clang-format --dry-run --Werror
