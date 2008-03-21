#ifndef __BUNIT_H__
#define __BUNIT_H__

#include "Energy.h"
#include <vector>

// BUnit - behavioral unit

typedef int node_t;

class BInterp;
class BUnit;
class Color;

BUnit* make_random_BUnit();

class BUnit {
    friend class ReproductionUnit;  // does some fancy stuff with its outputs

public:
    virtual ~BUnit() { }
    virtual void receive(BInterp* interp, int input, energy_t particle) = 0;
    virtual void randomize() = 0;
    virtual void mutate() = 0;
    virtual void die(BInterp* interp) = 0;   // release stored resources into the environment
    virtual void step(BInterp* interp) = 0;
    virtual double size() const = 0;
    virtual BUnit* clone() const = 0;
    virtual Color color() const = 0;

    int get_n_inputs()  const { return n_inputs_; }
    int get_n_outputs() const { return outputs_.size(); }
    std::pair<node_t, int> get_output(int n) const { 
        return outputs_[n];
    }
    void set_output(int n, node_t node, int input) { 
        outputs_[n] = std::make_pair(node, input);
    }

protected:
    BUnit(int n_inputs, int n_outputs)
        : n_inputs_(n_inputs), outputs_(n_outputs)
    { }
    
    void send(BInterp* interp, int output, energy_t particle);
    void copy_into(BUnit* target) const {
        target->n_inputs_ = n_inputs_;
        target->outputs_ = outputs_;
    }
    
private:
    int n_inputs_;
    std::vector< std::pair<node_t, int> > outputs_;
};

#endif
