#include <shared/array_texture.h>

#ifdef MACOS
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

using namespace glh;


GLenum array_texture_target = GL_TEXTURE_2D;

void make_rgba_texture(const array2<vec4ub> & rgb, bool mipmapped)
{
	if(mipmapped)
		gluBuild2DMipmaps(array_texture_target, GL_RGBA8, rgb.get_width(), rgb.get_height(), GL_RGBA,
						  GL_UNSIGNED_BYTE, (const void *)rgb.get_pointer());
	else
		glTexImage2D(array_texture_target, 0, GL_RGBA, rgb.get_width(), rgb.get_height(), 0,
					 GL_RGBA, GL_UNSIGNED_BYTE, (const void *)rgb.get_pointer());
}

void make_rgba_texture(const array2<vec3ub> & rgb,
                       const array2<unsigned char> & a, bool mipmapped)
{
	int w = rgb.get_width();
	int h = rgb.get_height();

	if(a.get_width() != w || a.get_height() != h) return; // !?

	GLubyte * img = new GLubyte[w*h*4];
	GLubyte * ip = img;

	for(int j=0; j < h; j++)
		for(int i=0; i < w; i++)
		{
			const vec3ub & v = rgb(i,j);
			*ip++ = v.v[0];
			*ip++ = v.v[1];
			*ip++ = v.v[2];
			*ip++ = a(i,j);
		}

	if(mipmapped)
		gluBuild2DMipmaps(array_texture_target, GL_RGBA8, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
	else
		glTexImage2D(array_texture_target, 0, GL_RGBA8, w, h, 0,
					 GL_RGBA, GL_UNSIGNED_BYTE, img);


	delete [] img;
}

void make_rgb_texture(const array2<vec3ub> & rgb, bool mipmapped)
{
	if(mipmapped)
		gluBuild2DMipmaps(array_texture_target, GL_RGB8, rgb.get_width(), rgb.get_height(), GL_RGB,
						  GL_UNSIGNED_BYTE, (const void *)rgb.get_pointer());
	else
		glTexImage2D(array_texture_target, 0, GL_RGB, rgb.get_width(), rgb.get_height(), 0,
					 GL_RGB, GL_UNSIGNED_BYTE, (const void *)rgb.get_pointer());
}

void make_scalar_texture(const array2<unsigned char> & s,
                         GLenum internal_format, GLenum format, bool mipmapped)
{
	if(mipmapped)
		gluBuild2DMipmaps(array_texture_target, internal_format, s.get_width(), s.get_height(), format,
						  GL_UNSIGNED_BYTE, (const void *)s.get_pointer());
	else
		glTexImage2D(array_texture_target, 0, internal_format, s.get_width(), s.get_height(), 0,
					 format, GL_UNSIGNED_BYTE, (const void *)s.get_pointer());
}

