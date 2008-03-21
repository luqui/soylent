#include "config.h"
#include "minesolve.h"
#include "drawFunctions.h"
#include "gameBoard.h"
#include <iostream>

#include <algorithm>

/*
 * The exhaustive search algorithm with agressive pruning.
 * Here's an example of how this algorithm works:
 *   Say we have the equations:
 *   
 *     a + b         = 1
 *     a + b + c     = 2
 *             c + d = 1
 *   
 *   The algorithm first tries a = 0.  Then it checks the equations with a
 *   substituted for "absurdity".  Suffice to say that it finds none (we'll see
 *   what it means soon).  Then it tries b = 0, and checks the equations with a
 *   and b substituted for absurdity:
 *   
 *     0 + 0         = 1
 *     0 + 0 + c     = 2
 *             c + d = 1
 *   
 *   The first two equations are found to be absurd.  A human can easily see
 *   that they will never be satisfied, but how does the computer know?
 *
 *   It checks to see if all the "free variables" (c and d in this case) were
 *   substituted to be 1, the left hand side is still less than the right hand
 *   side of each equation.  That is, the highest value the left side could
 *   achieve given the current bindings is still lower than the left.  This is
 *   what happened in the example.
 *
 *   It also checks to see if all the free variables were substituted to be 0,
 *   the left hand side is greater than the right hand side.  That is, the lowest
 *   value the left could achieve is still too big to satisfy the right side.
 *
 *   If it finds any equations to be absurd, as in this example, it backtracks and
 *   tries another binding.  Here it would "roll back" the b = 0 hypothesis and
 *   try b = 1.  If all variables have been bound without finding any absurdities,
 *   the solver pushes those bindings on to the list of solutions.
 */

static int evaluatePrefix(const Equation& eq, const vector<int>& prefix, int unknown) {
    int result = 0;
    for (vector<int>::const_iterator i = eq.xs.begin(); i != eq.xs.end(); ++i) {
        if (*i < prefix.size()) { result += prefix[*i]; }
        else                    { result += unknown; }
    }
    return result;
}

// a comparator that compares by indexing into a frequency table
struct FreqCmp {
    FreqCmp(const vector<int>& freq) : freq(freq) { }
    bool operator() (int x, int y) {
        return freq[y] < freq[x];
    }
    const vector<int>& freq;
};

static void permutation(int numvars, const list<Equation>& eqs, vector<int>* operm) {
    operm->clear();
    operm->resize(numvars);
    
    vector<int> iperm(numvars);

    vector<int> freq(numvars);
    for (int v = 0; v < numvars; v++) {
        int ct = 0;
        for (list<Equation>::const_iterator i = eqs.begin(); i != eqs.end(); ++i) {
            for (int j = 0; j < i->xs.size(); j++) {
                if (i->xs[j] == v) ct++;
            }
        }
        freq[v] = ct;
    }

    for (int i = 0; i < numvars; i++) iperm[i] = i;

    std::sort(iperm.begin(), iperm.end(), FreqCmp(freq));

    for (int i = 0; i < numvars; i++) (*operm)[iperm[i]] = i;
}

static bool checkEquations(const list<Equation>& eqs, const vector<int>& data) {
    for (list<Equation>::const_iterator eq = eqs.begin(); eq != eqs.end(); ++eq) {
        int lower = evaluatePrefix(*eq, data, 0);
        int upper = evaluatePrefix(*eq, data, 1);

        if (lower > eq->y) return false;
        if (upper < eq->y) return false;
    }
    return true;
}

static void solveGuts(
        int rem, const list<Equation>& eqs, vector<int>* prefix, 
		int minesleft, int* iters, vector<int>** ret) {

	if((*iters)++ % 1000 == 0) {
		getEvents();

		if (quit || *iters > bailout) {
			delete *ret;  *ret = 0;
			return;
		}
		else drawSearchingMessage();
	}

    if (0   > minesleft) return;
    
    if (checkEquations(eqs, *prefix)) {
        if (rem == 0) {
            vector<int>* pushee = new vector<int>(*prefix);
            if (*ret == 0) {
                *ret = pushee;
            }
            else {
                collapse(*ret, *pushee);
                delete pushee;
            }
        }
        else {
            prefix->push_back(0);
            solveGuts(rem - 1, eqs, prefix, minesleft, iters, ret);
            (*prefix)[prefix->size() - 1] = 1;
            solveGuts(rem - 1, eqs, prefix, minesleft - 1, iters, ret);
            prefix->pop_back();
        }
    }
}

class PruneSearchSolver : public MineSolver {
public:
    vector<int> solve(
            int numVars,
            const list<Equation>& equations,
            int minesLeft);
};

vector<int> PruneSearchSolver::solve(
        int numVars,
        const list<Equation>& equations,
        int minesLeft)
{
    vector<int> ret(numVars, -1);
    if (!equations.size()) return ret;

    list<Equation> eqs = equations;
    vector<int> prefix;
    vector<int>* output = 0;
    int iters = 0;
    
    vector<int> operm;
    permutation(numVars, eqs, &operm);
    
    for (list<Equation>::iterator i = eqs.begin(); i != eqs.end(); ++i) {
        for (int j = 0; j < i->xs.size(); j++) {
            i->xs[j] = operm[i->xs[j]];
        }
    }
    
    solveGuts(numVars, eqs, &prefix, minesLeft, &iters, &output);
    
    if (output) {
        for (int i = 0; i < numVars; i++) {
            ret[i] = (*output)[operm[i]];
        }
        delete output;
    }

    return ret;
}

auto_ptr<MineSolver> makePruneSearchSolver()
{
    return auto_ptr<MineSolver>(new PruneSearchSolver());
}
