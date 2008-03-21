CXXFLAGS = -g `sdl-config --cflags` -I/home/luqui/devel/soylent/open/soylib/include
LDLIBS = `sdl-config --libs` /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lGL -lGLU

HFILES = Trip.h Tripper.h
OFILES = main.o

all: main

clean:
	rm -f *.o

main: $(OFILES)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<
