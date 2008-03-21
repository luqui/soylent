CXXFLAGS = -Wall -I../common/manymouse -I../common -I../../open/soylib/include -g `sdl-config --cflags`
LDLIBS = `sdl-config --libs` -lGL -lGLU /home/luqui/devel/soylent/open/soylib/src/libsoy.so.1 -lode -lSDL_mixer

HFILES = Arena.h DuelMode.h GameMode.h physics.h Sword.h SwordController.h Stance.h Fighter.h MeatSack.h DrawQueue.h audio.h
OFILES = main.o ../common/libBen.o

all: main

clean:
	rm -f $(OFILES)
	make -C ../common clean

../common/libBen.o: 
	make -C ../common

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<

main: $(OFILES)
	$(CXX) $^ $(LDLIBS) -o $@
