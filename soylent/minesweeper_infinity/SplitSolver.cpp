#include "config.h"
#include "minesolve.h"
#include "CountableMap.h"
#include <set>
#include <iostream>

using std::set;

/* split independent regions into separate problems */

class SplitSolver : public MineSolver
{
public:
    SplitSolver(auto_ptr<MineSolver> child) : child(child) { }

    vector<int> solve(
            int numVars,
            const list<Equation>& equations,
            int minesLeft);

private:
    auto_ptr<MineSolver> child;
};

auto_ptr<MineSolver> makeSplitSolver(auto_ptr<MineSolver> child) {
    return auto_ptr<MineSolver>(new SplitSolver(child));
}

struct Region {
    set<int> deps;
    list<Equation> equations;
};

static bool intersects(const set<int>& a, const set<int>& b) {
    set<int>::const_iterator i = a.begin();
    set<int>::const_iterator j = b.begin();

    while (i != a.end() && j != b.end()) {
        if      (*i == *j) return true;
        else if (*i < *j)  ++i;
        else if (*i > *j)  ++j;
    }
    return false;
}

vector<int> SplitSolver::solve(
        int numVars,
        const list<Equation>& equations,
        int minesLeft)
{
    // Create a set for each equation, with that equation as
    // its singleton element.  Enqueue it.
    list<Region*> queue;
    for (list<Equation>::const_iterator i = equations.begin(); 
            i != equations.end(); ++i) {
        Region* r = new Region;
        r->equations.push_back(*i);
        for (int j = 0; j < i->xs.size(); j++) {
            r->deps.insert(i->xs[j]);
        }
        queue.push_back(r);
    }

    // Collect the lists of equations into disjoint sets of dependencies.
    for (list<Region*>::iterator i = queue.begin(); i != queue.end(); ) {
        int control = 0;
        list<Region*>::iterator j = i;
        for (++j; j != queue.end(); ++j) {
            if (intersects((*i)->deps, (*j)->deps)) {
                (*j)->deps.insert((*i)->deps.begin(), (*i)->deps.end());
                (*j)->equations.insert((*j)->equations.end(), 
                                       (*i)->equations.begin(), (*i)->equations.end());
                delete *i;

                list<Region*>::iterator itemp = i; ++itemp;
                queue.erase(i);
                i = itemp;
                control = 1;
                break;
            }
        }
        if (!control)  ++i;
    }
    
    vector<int> results(numVars, -1);
    
    // Pass each set of equations on to the child solver and merge 
    // the results.
    for (list<Region*>::iterator i = queue.begin(); i != queue.end(); ++i) {
        // Renumber variables
        CountableMap<int> renumbering;
        for (list<Equation>::iterator j = (*i)->equations.begin(); 
                j != (*i)->equations.end(); ++j) {
            for (vector<int>::iterator k = j->xs.begin(); k != j->xs.end(); ++k) {
                *k = renumbering.Get(*k);
            }
        }
        
        // Solve the subsystem
        vector<int> subresults = child->solve(
                                    renumbering.getSize(), 
                                    (*i)->equations,
                                    minesLeft);  // XXX minesLeft may not be correct
        
        delete *i;
        
        // Merge results
        for (int j = 0; j < subresults.size(); j++) {
            results[renumbering.getKey(j)] = subresults[j];
        }
    }

    return results;
}
