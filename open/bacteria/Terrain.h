#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "Energy.h"
#include <soy/Viewport.h>
#include <vector>

class Terrain {
public:
    Terrain(const Viewport& area, const std::vector<double>& distrib, double time) 
        : area_(area), distrib_(distrib), time_(time), timeleft_(0)
    { }

    static Terrain make_random(const Viewport& container) {
        vec2 ll = random_vector(container);
        vec2 ur = random_vector(container);
        if (ll.x > ur.x) { std::swap(ll.x, ur.x); }
        if (ll.y > ur.y) { std::swap(ll.y, ur.y); }

        std::vector<double> distrib(NUM_ENERGY_TYPES);
        double sum = 0;
        for (int i = 0; i < NUM_ENERGY_TYPES; i++) {
            distrib[i] = randrange(0,1);
            sum += distrib[i];
        }
        for (int i = 0; i < NUM_ENERGY_TYPES; i++) {
            distrib[i] /= sum;
        }

        double time = 1.0/randrange(1,100);

        return Terrain(Viewport::from_bounds(ll,ur), distrib, time);
    }

    void plot() {
        timeleft_ -= DT;
        while (timeleft_ <= 0) {
            double sel = randrange(0,1);
            double accum = 0;
            for (int i = 0; i < NUM_ENERGY_TYPES; i++) {
                accum += distrib_[i];
                if (sel < accum) {
                    PARTICLES.put(random_vector(area_), i);
                    break;
                }
            }
            timeleft_ += time_;
        }
    }

    void mutate() {
        area_ = Viewport::from_bounds(area_.ll() + vec2(randrange(-DT,DT),randrange(-DT,DT)),
                                      area_.ur() + vec2(randrange(-DT,DT),randrange(-DT,DT)));
        double sum = 0;
        for (int i = 0; i < NUM_ENERGY_TYPES; i++) {
            distrib_[i] += 0.1 * randrange(-DT,DT);
            if (distrib_[i] < 0) distrib_[i] = 0;
            sum += distrib_[i];
        }
        for (int i = 0; i < NUM_ENERGY_TYPES; i++) {
            distrib_[i] /= sum;
        }
    }

    void alter_speed(double amount) {
        time_ *= amount;
        std::cout << "time = DT/" << DT/time_ << "\n";
    }

private:

    Viewport area_;
    std::vector<double> distrib_;
    double time_;

    double timeleft_;
};

#endif
