CXXFLAGS = -O3 -g -Wall `sdl-config --cflags` -I/home/luqui/devel/soylent/open/soylib/include
LDLIBS = `sdl-config --libs` /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lGL -lGLU

HFILES = BInterp.h BUnit.h Color.h ConverterUnit.h Cell.h EjectorUnit.h Energy.h MergeUnit.h MotionUnit.h ParticleField.h ReproductionUnit.h Terrain.h tweak.h
OFILES = main.o BInterp.o BUnit.o Cell.o

all: main

clean:
	rm -f *.o

main: $(OFILES)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<
