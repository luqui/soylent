CXXFLAGS = $(BUILD_DEBUG) -O3 `sdl-config --cflags` -Wall -Wno-sign-compare
LDLIBS = `sdl-config --libs` -lSDL_gfx -lSDL_image -lguichan -lguichan_sdl -lSDL_ttf

OFILES = Font.o Interpreter.o drawFunctions.o gameBoard.o init.o main.o PruneSearchSolver.o SplitSolver.o Utils.o GUI.o Clock.o $(if $(BUILD_WINDOWS),mines.o,)
HFILES = CountableMap.h Font.h Interpreter.h config.h drawFunctions.h gameBoard.h init.h minesolve.h Utils.h GUI.h Clock.h HighScoreSheet.h

all: mines

clean:
	rm -f $(OFILES) mines.o

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $<

mines.o: mines.rc
	i386-mingw32msvc-windres mines.rc -o mines.o

mines: $(OFILES)
	$(CXX) -o $@ $^ $(LDLIBS)
