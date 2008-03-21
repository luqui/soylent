#ifndef __SOYLIB_ERROR_H__
#define __SOYLIB_ERROR_H__

#include "soy/config.h"
#include <string>

#define ONERROR == 0 ||
#define DIE(msg) _soylib_die(std::string() + msg)

int DLLEXPORT _soylib_die(const std::string& msg) __attribute__((noreturn));

#endif
