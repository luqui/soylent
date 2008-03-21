CXXFLAGS = -O3 `sdl-config --cflags` -I/home/luqui/devel/soylent/open/soylib/include
LDLIBS = `sdl-config --libs` /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lGL -lGLU

HFILES = Field.h Element.h Terrain.h
OFILES = main.o Terrain.o

all: main

clean:
	rm -f *.o

main: $(OFILES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<
