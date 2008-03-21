PROFILE =
CXXFLAGS = -Wall -I../../open/soylib/include -march=athlon-xp -g $(PROFILE) -O2 `sdl-config --cflags` -DNDEBUG
LDLIBS = `sdl-config --libs` $(if $(BUILD_WINDOWS),-lopengl32 -lglu32 -lglaux,-lGL -lGLU) -lglut ../../open/soylib/src/libsoy.so.1

HFILES = ControlRole.h GenerationalRule.h MainGame.h SelectorRole.h TwoPlayerStarWarsRule.h Universe.h \
		 Utils.h ViewBox.h config.h Player.h Library.h
OFILES = main.o

all: emergency

clean: 
	rm -f $(OFILES)

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<

emergency: $(OFILES)
	$(CXX) $(PROFILE) -o $@ $^ $(LDLIBS)
