#include "config.h"
#include <iostream>
#include <cstdlib>

const num C = 10;
const num ZL_GROUND = 0;
const num ZG_GROUND = 0;
const num ZL_TREES = 0;
const num ZG_TREES = 1;
const num ZL_SOLDIER = 0;
const num ZG_SOLDIER = 1;
const num FOREST_DENSITY = 0.2;
const num SOLVER_TOLERANCE = 0.001;

num CLOCK = 0;

int _die(const string& msg, const char* file, int line) {
    std::cerr << msg << " at " << file << " line " << line << "\n";
    SDL_Quit();
    std::exit(1);
}

void _log(const string& msg) {
    std::cout << msg << "\n";
}

num randrange(num a, num b)
{
    return (num(std::rand()) / RAND_MAX) * (b - a) + a;
}
