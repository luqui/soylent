#include "Interpreter.h"
#include "gameBoard.h"
#include "minesolve.h"
#include "CountableMap.h"
#include <memory>

using namespace std;
using std::auto_ptr;

struct pos {
	int x,y;
	pos(int x = 0, int y = 0):x(x),y(y){}
};

struct region {
    region(int llx, int lly, int urx, int ury) 
        : llx(llx), lly(lly), urx(urx), ury(ury) { }
    int llx, lly, urx, ury;
};

bool operator<(const pos& a, const pos& b) {
	if(a.x < b.x) return true;
	if(a.x > b.x) return false;
	return a.y < b.y;
}

void doFunctorPos(Equation& equat, const region& r, int x, int y, CountableMap<pos>& mapper) {
    if (x < r.llx || x >= r.urx || y < r.lly || y >= r.ury) return;
    if(minefield[x][y].covered && !minefield[x][y].flagged)
        equat.xs.push_back(mapper.Get(pos(x,y)));
}

Equation Equator(const region& r, int x, int y, CountableMap<pos>& mapper) {
	Equation equat;

	equat.y = minefield[x][y].displayNum;

	doFunctorPos(equat, r, x, y-1,   mapper);
	doFunctorPos(equat, r, x-1, y-1, mapper);
	doFunctorPos(equat, r, x-1, y,   mapper);
	doFunctorPos(equat, r, x-1, y+1, mapper);
	doFunctorPos(equat, r, x, y+1,   mapper);
	doFunctorPos(equat, r, x+1, y+1, mapper);
	doFunctorPos(equat, r, x+1, y,   mapper);
	doFunctorPos(equat, r, x+1, y-1, mapper);

	return equat;
}

bool SolveRegion(const region& r) {
    auto_ptr<MineSolver> solver = makeSplitSolver(makePruneSearchSolver());
    
	bool changed = false;
	CountableMap<pos> mapper;
	list<Equation> equatList;
	for(int i = 0; i < boardWidth; i++)
		for(int j = 0; j < boardHeight; j++)
			if(!minefield[i][j].covered && minefield[i][j].displayNum > 0)
				equatList.push_back(Equator(r,i,j,mapper));

	vector<int> solution = solver->solve(mapper.getSize(), equatList, numMines - flags);

	for(int i = 0; i < solution.size(); i++){
		if(solution[i] > -1){
			pos tempPos = mapper.getKey(i);
			minefield[tempPos.x][tempPos.y].changed = true;
			changed = true;
			if(solution[i] == 0){
				minefield[tempPos.x][tempPos.y].covered = false;
			}
			else{
                minefield[tempPos.x][tempPos.y].flagged = true;
                flags++;
                updateDisplayNum(tempPos.x,tempPos.y,true);
			}
		}
	}
	if(changed)
		for(int i = 0; i < boardWidth; i++)
			for(int j = 0; j < boardHeight; j++)
				if(!minefield[i][j].covered)revealBlocks(i,j);
	return changed;
}

bool Solver() {
    region whole(0, 0, boardWidth, boardHeight);
    return SolveRegion(whole);
}

bool testMinefield(){//tests to see if the minefield is solvable
	while(Solver()){
		SDL_Delay(3);
	}
	if(flags == numMines) return true;
	return false;
}
