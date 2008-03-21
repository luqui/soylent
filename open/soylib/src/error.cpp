#include "soy/error.h"
#include <iostream>
#include <cstdlib>

#ifdef WIN32
#  include <windows.h>
#endif

int DLLEXPORT _soylib_die(const std::string& msg)
{
#ifdef WIN32
	MessageBox(NULL, msg.c_str(), NULL, MB_OK | MB_ICONERROR);
#else
	std::cerr << "Error: " << msg << std::endl;
#endif
	std::exit(1);
}
