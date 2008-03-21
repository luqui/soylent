CXXFLAGS = -I../include -fPIC -Wall -O2 -g `sdl-config --cflags`
LDLIBS = `sdl-config --libs` -lGL -lGLU -lSDL_image -lexpat -lSDL_ttf

OFILES = error.o init.o Texture.o drawing.o weak.o
INCDIR = ../include/soy
HFILES = $(INCDIR)/Texture.h $(INCDIR)/Timer.h $(INCDIR)/Viewport.h \
		 $(INCDIR)/drawing.h $(INCDIR)/error.h $(INCDIR)/init.h \
		 $(INCDIR)/ptr.h     $(INCDIR)/util.h  $(INCDIR)/vec2.h \
		 $(INCDIR)/vec3.h    $(INCDIR)/weak.h  $(INCDIR)/Floater.h

all: libsoy.so.1

%.o: %.cpp $(HFILES)
	$(CXX) -c $(CXXFLAGS) $< -o $@

libsoy.so.1.0: $(OFILES)
	$(CXX) -g -shared -Wl,-soname,libsoy.so.1 -o libsoy.so.1.0 $^ $(LDLIBS)

libsoy.so.1: libsoy.so.1.0
	ln -sf $< $@
