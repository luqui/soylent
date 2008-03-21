#include <stdio.h>
#include <string.h>

#ifdef WIN32
# include <windows.h>
#endif

#ifdef MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <glh/glh_array.h>
#include <glh/glh_linear.h>

#include <shared/load_cubemap.h>

#include <shared/array_texture.h>
#include <shared/nv_png.h>

using namespace glh;

void load_png_cubemap(const char * string, bool mipmap)
{
	char buff[1024];
	GLenum tgt = array_texture_target;

	array2<vec3ub> cubeface;
	
	sprintf(buff, string, "posx");
	read_png_rgb(buff, cubeface);
	array_texture_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
	make_rgb_texture(cubeface, mipmap);

	sprintf(buff, string, "negx");
	read_png_rgb(buff, cubeface);
	array_texture_target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
	make_rgb_texture(cubeface, mipmap);

	sprintf(buff, string, "posy");
	read_png_rgb(buff, cubeface);
	array_texture_target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
	make_rgb_texture(cubeface, mipmap);

	sprintf(buff, string, "negy");
	read_png_rgb(buff, cubeface);
	array_texture_target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
	make_rgb_texture(cubeface, mipmap);

	sprintf(buff, string, "posz");
	read_png_rgb(buff, cubeface);
	array_texture_target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
	make_rgb_texture(cubeface, mipmap);

	sprintf(buff, string, "negz");
	read_png_rgb(buff, cubeface);
	array_texture_target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
	make_rgb_texture(cubeface, mipmap);

	array_texture_target = tgt;
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}
