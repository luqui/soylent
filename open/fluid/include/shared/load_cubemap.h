#ifndef LOAD_CUBEMAP_H
#define LOAD_CUBEMAP_H

/*
	convenience file for loading up a set of cubemap images
*/

// string should be of the form "path/to/cubemaps_%s.png"
// where %s is replaced with "(pos|neg)(x|y|z)"
void load_png_cubemap(const char * string, bool mipmap);


#endif
