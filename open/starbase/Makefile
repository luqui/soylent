CXXFLAGS = -g `sdl-config --cflags` -I/home/luqui/devel/soylent/open/soylib/include
LDLIBS = `sdl-config --libs` /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lGL -lGLU -lode

HFILES = GameObject.h Energy.h Component.h PowerComponent.h Pickable.h ContextMenu.h physics.h game.h
OFILES = main.o Component.o

all: main

clean:
	rm -f *.o

main: $(OFILES)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<
