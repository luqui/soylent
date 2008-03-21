CXXFLAGS = -I../soylib/include -g `sdl-config --cflags`
LDLIBS = `sdl-config --libs` ../soylib/src/libsoy.so.1 -lSDL_mixer

OFILES = main.o levels.o
HFILES = Cell.h Dude.h MouseInterpreter.h config.h Viewport.h levels.h Music.h Level.h Graphics.h

%.o: %.cc $(HFILES)
	g++ -c -o $@ $(CXXFLAGS) $<

main: $(OFILES) $(HFILES)
	g++ -o $@ $(CXXFLAGS) $(OFILES) $(LDLIBS)
