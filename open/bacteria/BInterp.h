#ifndef __BINTERP_H__
#define __BINTERP_H__

#include "BUnit.h"
#include "Color.h"
#include <vector>
#include <queue>
#include <soy/util.h>
#include <cmath>

class Cell;

class BInterp {
public:
    BInterp(Cell* cell) : cell_(cell), top_(0), top_input_(0) { }

    ~BInterp() {
        for (int i = 0; i < int(units_.size()); i++) {
            delete units_[i];
        }
    }

    double size() const {
        double r = 0;
        for (int i = 0; i < int(units_.size()); i++) {
            if (units_[i]) { r += units_[i]->size(); }
        }

        return sqrt(double(r+1));
    }

    std::vector<Color> colors() const {
        std::vector<Color> r;
        for (int i = 0; i < int(units_.size()); i++) {
            if (units_[i]) { r.push_back(units_[i]->color()); }
        }
        return r;
    }
    
    void send(node_t target, int input, energy_t particle) {
        // target 0 represents empty node
        assert(target >= 0);
        if (target) {
            messages_.push(Message(target-1, input, particle));
        }
        else {
            release(particle);
        }
    }

    void absorb(energy_t particle) {
        send(top_, top_input_, particle);
    }

    void release(energy_t particle);

    void step() {
        pump_messages();
        for (int i = 0; i < int(units_.size()); i++) {
            if (units_[i]) {
                units_[i]->step(this);
            }
        }
    }

    BInterp* clone(node_t top, int top_input) const {
        assert(top >= 0);
        BInterp* n = new BInterp(0);
        n->units_.resize(units_.size());
        for (int i = 0; i < int(units_.size()); i++) {
            if (units_[i]) {
                n->units_[i] = units_[i]->clone();
            }
        }
        n->top_ = top;
        n->top_input_ = top_input;
        return n;
    }

    void randomize() {
        // make a bunch of random nodes
        int num_nodes = rand() % (INITIAL_NODE_MAX - INITIAL_NODE_MIN) + INITIAL_NODE_MIN;
        units_.resize(num_nodes);
        for (int i = 0; i < num_nodes; i++) {
            units_[i] = make_random_BUnit();
        }

        // go through the nodes and make random connections
        for (int i = 0; i < num_nodes; i++) {
            int nconn = units_[i]->get_n_outputs();
            for (int j = 0; j < nconn; j++) {
                if (randrange(0,1) < INITIAL_CONNECTION_PROBABILITY) {
                    // find a target node
                    node_t target = rand() % num_nodes + 1;
                    // find a target input
                    int input = rand() % units_[i]->get_n_inputs();
                    // make the connection
                    units_[i]->set_output(j, target, input);
                }
            }
        }

        // choose a random top
        top_ = rand() % num_nodes + 1;
        top_input_ = rand() % units_[top_-1]->get_n_inputs();
    }

    void mutate() {
        if (randrange(0,1) < CONTENT_MUTATION_PROBABILITY) {
            do {
                node_t src_node = rand() % units_.size();
                BUnit* src = units_[src_node];
                if (!src) break;
                src->mutate();
            } while (0);
        }

        if (randrange(0,1) < NEW_CONNECTION_PROBABILITY) {
            do {
                node_t src_node = rand() % units_.size();
                BUnit* src = units_[src_node];
                if (!src) break;
                if (src->get_n_outputs() == 0) break;
                int src_output = rand() % src->get_n_outputs();
                node_t dest_node = rand() % units_.size();
                BUnit* dest = units_[dest_node];
                if (!dest) break;
                if (dest->get_n_inputs() == 0) break;
                int dest_input = rand() % dest->get_n_inputs();
                src->set_output(src_output, dest_node+1, dest_input);
            } while (0);
        }

        if (randrange(0,1) < ALTER_TOP_PROBABILITY) {
            do {
                node_t newtop = rand() % units_.size();
                if (!units_[newtop]) break;
                if (units_[newtop]->get_n_inputs() == 0) break;

                top_ = newtop;
                top_input_ = rand() % units_[top_]->get_n_inputs();
            } while (0);
        }

        if (randrange(0,1) < BREAK_CONNECTION_PROBABILITY) {
            do {
                node_t src_node = rand() % units_.size();
                BUnit* src = units_[src_node];
                if (!src) break;
                if (src->get_n_outputs() == 0) break;
                int src_output = rand() % src->get_n_outputs();
                src->set_output(src_output, 0, 0);
            } while (0);
        }

        if (randrange(0,1) < REPLACE_NODE_PROBABILITY) {
            do {
                node_t src_node = rand() % units_.size();
                BUnit* src = units_[src_node];
                BUnit* n = make_random_BUnit();

                if (src) {
                    int nouts = std::min(src->get_n_outputs(), n->get_n_outputs());
                    for (int i = 0; i < nouts; i++) {
                        std::pair<node_t,int> conn = src->get_output(i);
                        n->set_output(i, conn.first, conn.second);
                    }
                }
                units_[src_node] = n;
                delete src;
            } while (0);
        }

        if (randrange(0,1) < DELETE_NODE_PROBABILITY) {
            do {
                node_t src_node = rand() % units_.size();
                BUnit* src = units_[src_node];
                units_[src_node] = NULL;
                delete src;
            } while (0);
        }

        if (randrange(0,1) < ADD_NODE_SLOT_PROBABILITY) {
            do {
                units_.push_back(make_random_BUnit());
            } while (0);
        }
    }

    void die() {
        for (int i = 0; i < int(units_.size()); i++) {
            if (units_[i]) {
                units_[i]->die(this);
            }
        }

        while (!messages_.empty()) {
            release(messages_.front().particle);
            messages_.pop();
        }
    }

    Cell* cell() const { return cell_; }

    void set_parent(Cell* parent) { cell_ = parent; }

private:
    void pump_messages() {
        if (messages_.empty()) return;
        Message m = messages_.front();
        messages_.pop();

        if (m.target < int(units_.size())) {
            BUnit* unit = units_[m.target];
            if (unit && m.input < unit->get_n_inputs()) {
                unit->receive(this, m.input, m.particle);
                return;
            }
        }

        release(m.particle);
    }

    struct Message {
        Message(node_t target, int input, energy_t particle)
            : target(target), input(input), particle(particle)
        { }

        node_t target;
        int input;
        energy_t particle;
    };

    Cell* cell_;
    node_t top_;
    int top_input_;
    std::queue<Message> messages_;
    std::vector<BUnit*> units_;
    
    // no copying
    BInterp(const BInterp&);
    BInterp& operator = (const BInterp&);
};

#endif
