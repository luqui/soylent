HFILES = Actor.h Widget.h config.h vec.h Texture.h Terrain.h Draw.h Soldier.h Messages.h Relativity.h Board.h

OFILES = Actor.o config.o main.o Texture.o Terrain.o Draw.o Soldier.o Messages.o Relativity.o Board.o

CXXFLAGS = -O -Wall -Wno-sign-compare -g `sdl-config --cflags`
LDLIBS   = `sdl-config --libs` -lGL -lGLU -lgc -lSDL_image

strategy: $(OFILES)
	g++ $(CXXFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o

%.o: %.cpp $(HFILES) Makefile
	g++ $(CXXFLAGS) -c $<

