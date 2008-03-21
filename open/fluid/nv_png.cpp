#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <shared/nv_png.h>
#include <png/png.h>

#define FAILMSGANDRETURN \
{\
	fprintf(stderr,"Cannot open \"%s\" for read!\n", filename);\
	return;\
}
#ifdef WIN32

#define PNG_FREERESOURCE if(hglobal) FreeResource(hglobal);
#define PNG_DECLARE_RESVAR \
	DWORD	ressz;\
	HRSRC	HRes	= NULL;\
	HGLOBAL hglobal = NULL;\
	void*	resptr;
#define PNG_SETREADFN \
	if(!fp) png_set_read_fn(png_ptr, (void *)resptr, png_read_resource_fn);	 else
#define PNG_READHEADER \
	if(!fp)\
	{\
		memcpy(sig, resptr, 8);\
		resptr = (void*)((char*)resptr + 8);\
	} else

#else

#define PNG_FREERESOURCE 
#define PNG_DECLARE_RESVAR 
#define PNG_SETREADFN 
#define PNG_READHEADER

#endif


using namespace glh;

namespace 
{
	data_path path;
#ifdef WIN32
	struct png_resource_access
	{
		png_resource_access() : hModule(NULL), res_type_id(0) {}
		HMODULE hModule;
		std::string res_type_name;
		unsigned long res_type_id;
	};
	png_resource_access png_resource;
#endif
}

void png_read_resource_fn(png_structp png_ptr, png_bytep data, png_size_t leng);

#ifdef WIN32
void set_png_module_handle(unsigned long hM) 
{ 
	png_resource.hModule = (HMODULE)hM; 
}
void set_png_module_restypename(const char * restypename) 
{
	if(HIWORD(restypename))
	{
		png_resource.res_type_name = restypename;
		png_resource.res_type_id = 0;
	}
	else
	{
		//resource.res_type_name.clear(); should exist (cf STL Doc)
		png_resource.res_type_id = (unsigned long)restypename;
	}
}
#endif
data_path get_png_path() { return path; }
void      set_png_path(const data_path & newpath) { path = newpath; }

#ifdef WIN32
HRSRC get_resource(const char * filename)
{
    HRSRC HRes;
#ifdef _UNICODE
    unsigned int count = strlen(filename);
    wchar_t *wfilename = new wchar_t[count];
    mbstowcs(wfilename, filename, count);
    
    count = png_resource.res_type_name.size();
    wchar_t *wres_type_name = new wchar_t[count];
    mbstowcs(wres_type_name, png_resource.res_type_name.c_str(), count);

    HRes = FindResource(png_resource.hModule, wfilename, 
                        png_resource.res_type_id ? (LPCWSTR)png_resource.res_type_id : wres_type_name);

    delete [] wfilename;
    delete [] wres_type_name;
#else
    HRes = FindResource(png_resource.hModule, filename, 
                        png_resource.res_type_id ? (LPCSTR)png_resource.res_type_id : png_resource.res_type_name.c_str());
#endif
    return HRes;
}
#endif

void read_png_rgba(const char * filename, glh::array2<glh::vec4ub> & image)
{
	//--------- Resource stuff
	PNG_DECLARE_RESVAR
	//---------
	FILE * fp;
    png_byte sig[8];
    int bit_depth, color_type;
    double              gamma;
    png_uint_32 channels, row_bytes;
	png_structp png_ptr = 0;
    png_infop info_ptr = 0;


	// This is the SDK default path...
	if(path.path.size() < 1)
	{
		path.path.push_back(""); // added by Ashu, for case where fully qualified path is given
		path.path.push_back(".");
		path.path.push_back("Media/textures/1D");
		path.path.push_back("Media/textures/2D");
		path.path.push_back("Media/textures/rectangles");
		path.path.push_back("Media/textures/cubemaps");
		path.path.push_back("Media/textures/3D");
	}

    // open the PNG input file
    if (!filename) return;

    if (!(fp = path.fopen(filename)))
#ifdef WIN32
	{
		// Try resource access
        HRes = get_resource(filename);
            
        if(!HRes)
			FAILMSGANDRETURN
		hglobal	=	LoadResource(png_resource.hModule, HRes);
		if(!hglobal) 
			FAILMSGANDRETURN
		ressz = SizeofResource(png_resource.hModule, HRes);
		resptr =	(char*)LockResource(hglobal);
		if(!resptr) 
			FAILMSGANDRETURN
	}
#else
	FAILMSGANDRETURN
#endif
	
    // first check the eight byte PNG signature
	PNG_READHEADER fread(sig, 1, 8, fp);
    if (!png_check_sig(sig, 8)) { 
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
		return; 
	}
	
	
	// start back here!!!!
	
    // create the two png(-info) structures
	
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) { 
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
		return; 
	}
	
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, 0, 0);
		if(fp) fclose(fp);
		PNG_FREERESOURCE
    }
	
	// initialize the png structure
    PNG_SETREADFN png_init_io(png_ptr, fp);	

	png_set_sig_bytes(png_ptr, 8);
	
	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);
	
	// get width, height, bit-depth and color-type	
	unsigned long w, h;
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
	
	// expand images of all color-type and bit-depth to 3x8 bit RGB images
	// let the library process things like alpha, transparency, background
	
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (bit_depth < 8) png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);
	
	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &gamma)) png_set_gamma(png_ptr, (double) 2.2, gamma);
	
	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);
	
	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
	
	
	// row_bytes is the width x number of channels
	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
		
    if ( channels == 3 )
    {
		fprintf(stderr,"Unable to open image of type RGB \"%s\" using read_png_rgba().\n", filename);
		fprintf(stderr,"Try using read_png_rgb() instead!\n\n");
		return;
    }

    // now we can allocate memory to store the image

    png_byte * img = new png_byte[row_bytes * h];
	
	// and allocate memory for an array of row-pointers

	png_byte ** row = new png_byte * [h];

	
	// set the individual row-pointers to point at the correct offsets
	
	for (unsigned int i = 0; i < h; i++)
		row[i] = img + i * row_bytes;
	
	// now we can go ahead and just read the whole image
	
	png_read_image(png_ptr, row);
	
	// read the additional chunks in the PNG file (not really needed)
	
	png_read_end(png_ptr, NULL);
	
	image = array2<vec4ub>(w, h);

	{
		for(unsigned int i=0; i < w; i++)
			for(unsigned int j=0; j < h; j++)
			{ image(i,j) = vec4ub(img + ((h-j-1)*row_bytes + i * 4)); }
	}

	delete [] row;
	delete [] img;

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    if(fp) fclose (fp);
	PNG_FREERESOURCE
}

void read_png_rgb(const char * filename, glh::array2<glh::vec3ub> & image)
{
	//--------- Resource stuff
	PNG_DECLARE_RESVAR
	//----------
	FILE * fp;
    png_byte sig[8];
    int bit_depth, color_type;
    double              gamma;
    png_uint_32 channels, row_bytes;
	png_structp png_ptr = 0;
    png_infop info_ptr = 0;


	// This is the SDK default path...
	if(path.path.size() < 1)
	{
		path.path.push_back(""); // added by Ashu, for case where fully qualified path is given
		path.path.push_back(".");
		path.path.push_back("Media/textures/1D");
		path.path.push_back("Media/textures/2D");
		path.path.push_back("Media/textures/rectangles");
		path.path.push_back("Media/textures/cubemaps");
		path.path.push_back("Media/textures/3D");
	}

    // open the PNG input file
    if (!filename) return;
	
    if (!(fp = path.fopen(filename)))
#ifdef WIN32
	{
		// Try resource access
        HRes = get_resource(filename);

		if(!HRes)
			FAILMSGANDRETURN
		hglobal	=	LoadResource(png_resource.hModule, HRes);
		if(!hglobal) 
			FAILMSGANDRETURN
		ressz = SizeofResource(png_resource.hModule, HRes);
		resptr =	(char*)LockResource(hglobal);
		if(!resptr) 
			FAILMSGANDRETURN
	}
#else
	FAILMSGANDRETURN
#endif
	
    // first check the eight byte PNG signature
	PNG_READHEADER fread(sig, 1, 8, fp);
    if (!png_check_sig(sig, 8)) { 
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
		return; 
	}
	
	
	// start back here!!!!
	
    // create the two png(-info) structures
	
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) { 
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
		return; 
	}
	
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, 0, 0);
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
    }
	
	// initialize the png structure
	PNG_SETREADFN png_init_io(png_ptr, fp);	

	png_set_sig_bytes(png_ptr, 8);
	
	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);
	
	// get width, height, bit-depth and color-type	
	unsigned long w, h;
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
	
	// expand images of all color-type and bit-depth to 3x8 bit RGB images
	// let the library process things like alpha, transparency, background
	
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (bit_depth < 8) png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_strip_alpha(png_ptr);
	
	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &gamma)) png_set_gamma(png_ptr, (double) 2.2, gamma);
	
	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);
	
	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
	
	
	// row_bytes is the width x number of channels
	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);

    if ( channels == 4 )
    {
		fprintf(stderr,"Unable to open image of type RGBA \"%s\" using read_png_rgb().\n", filename);
		fprintf(stderr,"Try using read_png_rgba() instead!\n\n");
		return;
    }
		
	// now we can allocate memory to store the image

    png_byte * img = new png_byte[row_bytes * h];
	
	// and allocate memory for an array of row-pointers

	png_byte ** row = new png_byte * [h];

	
	// set the individual row-pointers to point at the correct offsets
	
	for (unsigned int i = 0; i < h; i++)
		row[i] = img + i * row_bytes;
	
	// now we can go ahead and just read the whole image
	
	png_read_image(png_ptr, row);
	
	// read the additional chunks in the PNG file (not really needed)
	
	png_read_end(png_ptr, NULL);
	
	image = array2<vec3ub>(w, h);

	{
		for(unsigned int i=0; i < w; i++)
			for(unsigned int j=0; j < h; j++)
			{ image(i,j) = vec3ub(img + ((h-j-1)*row_bytes + i * 3)); }
	}

	delete [] row;
	delete [] img;

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    if(fp) fclose (fp);
	PNG_FREERESOURCE
}


//FIXME: LIBPNG expands to RGB and only R is fetched...
void read_png_grey(const char * filename, glh::array2<unsigned char> & image)
{
	//--------- Resource stuff
	PNG_DECLARE_RESVAR
	//---------
	FILE * fp;
    png_byte sig[8];
    int bit_depth, color_type;
    double              gamma;
    png_uint_32 channels, row_bytes;
	png_structp png_ptr = 0;
    png_infop info_ptr = 0;


	// This is the SDK default path...
	if(path.path.size() < 1)
	{
		path.path.push_back(""); // added by Ashu, for case where fully qualified path is given
		path.path.push_back(".");
		path.path.push_back("Media/textures/1D");
		path.path.push_back("Media/textures/2D");
		path.path.push_back("Media/textures/rectangles");
		path.path.push_back("Media/textures/cubemaps");
		path.path.push_back("Media/textures/3D");
	}
	
    // open the PNG input file
    if (!filename) return;
	
    if (!(fp = path.fopen(filename)))
#ifdef WIN32
	{
		// Try resource access
        HRes = get_resource(filename);

        if(!HRes)
			FAILMSGANDRETURN
		hglobal	=	LoadResource(png_resource.hModule, HRes);
		if(!hglobal) 
			FAILMSGANDRETURN
		ressz = SizeofResource(png_resource.hModule, HRes);
		resptr =	(char*)LockResource(hglobal);
		if(!resptr) 
			FAILMSGANDRETURN
	}
#else
	FAILMSGANDRETURN
#endif
	
    // first check the eight byte PNG signature
	PNG_READHEADER fread(sig, 1, 8, fp);
    if (!png_check_sig(sig, 8)) { 
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
		return; 
	}
	
	
	// start back here!!!!
	
    // create the two png(-info) structures
	
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) { 
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
		return; 
	}
	
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, 0, 0);
		if(fp) fclose(fp); 
		PNG_FREERESOURCE
    }
	
	// initialize the png structure
    PNG_SETREADFN png_init_io(png_ptr, fp);	

	png_set_sig_bytes(png_ptr, 8);
	
	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);
	
	// get width, height, bit-depth and color-type	
	unsigned long w, h;
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
	
	// expand images of all color-type and bit-depth to 3x8 bit RGB images
	// let the library process things like alpha, transparency, background
	
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (bit_depth < 8) png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);
	
	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &gamma)) png_set_gamma(png_ptr, (double) 2.2, gamma);
	
	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);
	
	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
	
	
	// row_bytes is the width x number of channels
	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
		
	// now we can allocate memory to store the image

    png_byte * img = new png_byte[row_bytes * h];
	
	// and allocate memory for an array of row-pointers

	png_byte ** row = new png_byte * [h];

	
	// set the individual row-pointers to point at the correct offsets
	
	for (unsigned int i = 0; i < h; i++)
		row[i] = img + i * row_bytes;
	
	// now we can go ahead and just read the whole image
	
	png_read_image(png_ptr, row);
	
	// read the additional chunks in the PNG file (not really needed)
	
	png_read_end(png_ptr, NULL);
	
	image = array2<unsigned char>(w, h);

	{
		for(unsigned int i=0; i < w; i++)
			for(unsigned int j=0; j < h; j++)
			{ image(i,j) = *(img + ((h-j-1)*row_bytes + i * 3)); }
	}

	delete [] row;
	delete [] img;

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    if(fp) fclose (fp);
	PNG_FREERESOURCE
}

/**
	  read callback used by png library when using resource
 **/
void png_read_resource_fn(png_structp png_ptr, png_bytep data, png_size_t leng)
{
   png_bytep src = (png_bytep)png_ptr->io_ptr;

   for(unsigned int i=0; i<leng; i++)
	   data[i] = *src++;
	png_ptr->io_ptr = (void*)src;
}

