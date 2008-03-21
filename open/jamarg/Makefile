CXXFLAGS = -g -Wall `sdl-config --cflags` -I/home/luqui/devel/soylent/open/soylib/include
LDLIBS = `sdl-config --libs` /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lGL -lGLU -lSDL_mixer

HFILES = GameMode.h RecorderMode.h PlaybackMode.h
OFILES = main.o RecorderMode.o PlaybackMode.o

all: main

clean:
	rm -f *.o

main: $(OFILES)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<
