OFILES = Archivable.o Controllable.o FlockController.o InputNexus.o \
		 ManyMouseInput.o NVThing.o SDLInput.o manymouse/manymouse_all.o

CFLAGS = -g `sdl-config --cflags` -Imanymouse
CXXFLAGS = $(CFLAGS)

all: libBen.o

clean:
	rm -f $(OFILES)
	make -C manymouse clean

manymouse/manymouse_all.o: 
	CFLAGS=-DSUPPORT_XINPUT=0 make -C manymouse

libBen.o: $(OFILES)
	ld -r -o $@ $^
