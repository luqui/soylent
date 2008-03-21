CXXFLAGS = -g `sdl-config --cflags` -I/home/luqui/devel/soylent/open/soylib/include
LDLIBS = `sdl-config --libs` /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lGL -lGLU -lode -lglut -lSDL_mixer -lfluidsynth

HFILES = GameMode.h physics.h Platform.h PlatformMode.h Player.h drawing.h Metronome.h Synth.h Musician.h Sound.h
OFILES = main.o physics.o Synth.o

all: main

clean:
	rm -f *.o

main: $(OFILES)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<
