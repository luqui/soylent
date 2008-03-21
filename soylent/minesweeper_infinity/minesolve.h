#ifndef __MINESOLVE_H__
#define __MINESOLVE_H__

#include <list>
#include <vector>
#include <memory>
using std::list;
using std::vector;
using std::auto_ptr;

struct Equation {
    vector<int> xs;
    int y;
};

class MineSolver {
public:
    virtual ~MineSolver() { }
    virtual vector<int> solve(
            int numVars,
            const list<Equation>& equations,
            int minesLeft) = 0;
};

auto_ptr<MineSolver> makePruneSearchSolver();
auto_ptr<MineSolver> makeSplitSolver(auto_ptr<MineSolver> child);

inline void collapse(vector<int>* solution, const vector<int>& b){
	for(int i = 0; i < b.size(); i++)
		if((*solution)[i] != b[i]) (*solution)[i] = -1;
}

#endif
