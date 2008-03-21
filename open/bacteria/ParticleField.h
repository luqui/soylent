#ifndef __PARTICLEFIELD_H__
#define __PARTICLEFIELD_H__

#include "Energy.h"
#include "Cell.h"
#include <list>
#include <map>

class ParticleField
{
private:
    typedef std::pair<int,int> bincoord_t;
    typedef std::list<Particle> particles_t;
    struct Bin {
        Bin() : active(false) { }
        particles_t bin;
        bool active;
    };
    typedef std::map< bincoord_t, Bin > bins_t;

public:
    ParticleField(double binsize) : binsize_(binsize)
    { }

    void put(vec2 pos, energy_t particle, vec2 vel = vec2()) {
        // find the bin to put it in
        bincoord_t bin = quantize(pos);
        Bin& b = bins_[bin];
        bins_[bin].bin.push_back(Particle(pos, particle, vel));
        if (vel.norm2() > PARTICLE_FIELD_ACTIVE_THRESHOLD) {
            b.active = true;
        }
    }

    void step() {
        bins_t moved;

        for (bins_t::iterator i = bins_.begin(); i != bins_.end();) {
            Bin& bin = i->second;
            if (!bin.active) { ++i; continue; }
            bin.active = false;

            for (particles_t::iterator j = bin.bin.begin(); j != bin.bin.end();) {
                j->step(&bin.active);
                bincoord_t newbin = quantize(j->position());
                if (newbin != i->first) {
                    Bin& nb = moved[newbin];
                    nb.bin.push_back(*j);
                    nb.active = true;

                    particles_t::iterator k = j;
                    ++j;
                    bin.bin.erase(k);
                }
                else {
                    ++j;
                }
            }

            // remove empty bins
            if (i->second.bin.size() == 0) {
                bins_t::iterator k = i;
                ++i;
                bins_.erase(k);
            }
            else {
                ++i;
            }
        }

        // now merge the moved particles in
        for (bins_t::iterator i = moved.begin(); i != moved.end(); ++i) {
            Bin& bin = bins_[i->first];
            bin.active = bin.active || i->second.active;

            for (particles_t::iterator j = i->second.bin.begin(); j != i->second.bin.end(); ++j) {
                bin.bin.push_back(*j);
            }
        }
    }

    void draw() const {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPointSize(1.0);
        glBegin(GL_POINTS);
        for (bins_t::const_iterator i = bins_.begin(); i != bins_.end(); ++i) {
            for (particles_t::const_iterator j = i->second.bin.begin(); j != i->second.bin.end(); ++j) {
                j->draw_pt();
            }
        }
        glEnd();

        glDisable(GL_BLEND);
    }

    void absorb(Cell* cell) {
        bincoord_t bin = quantize(cell->position());

        int r = int(ceil(cell->radius() / binsize_));
        for (int dx = -r; dx <= r; dx++) {
            for (int dy = -r; dy <= r; dy++) {
                absorb_in_bin(std::make_pair(bin.first+dx, bin.second+dy), cell);
            }
        }
    }

private:
    void absorb_in_bin(const bincoord_t& bin, Cell* cell) {
        bins_t::iterator i = bins_.find(bin);
        if (i == bins_.end()) return;

        particles_t& ps = i->second.bin;

        vec2 pos = cell->position();
        double radius = cell->radius();
        for (particles_t::iterator i = ps.begin(); i != ps.end();) {
            if ((i->position() - pos).norm2() < radius*radius) {
                cell->absorb(i->type());
                particles_t::iterator j = i;
                ++i;
                ps.erase(j);
            }
            else {
                ++i;
            }
        }
    }


    bincoord_t quantize(const vec2 p) {
        return std::make_pair(int(floor(p.x / binsize_)), int(floor(p.y / binsize_)));
    }

    bins_t bins_;

    double binsize_;
};

#endif
