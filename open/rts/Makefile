CXXFLAGS = -Wall -I. -I../soylib/include -I/usr/lib/ruby/1.8/i686-linux -g `sdl-config --cflags`
LDLIBS = `sdl-config --libs` ../soylib/src/libsoy.so.1 -L/usr/lib/ruby/1.8/i686-linux -lruby

OFILES = main.o Cell.o PathFind.o TerrainGenerator.o UI.o ruby_interop.o
HFILES = Board.h Unit.h Cell.h config.h UnitSpeedAptitude.h util.h PathFind.h \
		 TerrainGenerator.h Dust.h Scheduler.h UI.h ruby_interop.h \
		 Units/Hummer.h

%.o: %.cc $(HFILES)
	g++ -c -o $@ $(CXXFLAGS) $<

all: main

main: $(OFILES) $(HFILES)
	g++ -o $@ $(CXXFLAGS) $(OFILES) $(LDLIBS)

clean:
	rm -f $(OFILES)
