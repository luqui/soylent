#ifndef __ENERGY_H__
#define __ENERGY_H__

enum EnergyType {
    ENERGY_RED,
    ENERGY_GREEN,
    ENERGY_YELLOW,
    ENERGY_BLUE,

    ENERGY_N_TYPES
};

class EnergyVector
{
public:
    EnergyVector() { 
        for (int i = 0; i < ENERGY_N_TYPES; i++) {
            vector_[i] = 0;
        }
    }

    double operator [] (EnergyType type) const { return vector_[type]; }

private:
    double vector_[ENERGY_N_TYPES];
};

#endif
