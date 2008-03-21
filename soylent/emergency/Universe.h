#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include <vector>
#include <utility>
#include <soy/vec2.h>
#include <soy/ptr.h>
#include <soy/util.h>
#include "Library.h"

using std::vector;

// Universe's job is to keep track of all the cells in the universe.
// It takes as a template parameter a Rule class.  Any
// Rule class must obey the following interface:
//   The *type* Rule::State exists, is a value type, and has a default 
//     constructor.  This is the type of thing that goes in each
//     grid cell.
//   The *type* Rule::StateBag exists, and has a member named
//     add which takes a State as its parameter.  This is used
//     to count up the neighbors of a cell.
//   The method Rule::make_state_bag() exists, which returns
//     an empty StateBag.
//   The method Rule::next_state(State, StateBag) exists, which
//     returns the state that a cell will be in next frame
//     if it was in state State and has StateBag neighbors.
//   The method Rule::draw(float x, float y, State state),
//     which draws a cell in state state at position (x,y).
//
// See the GenerationalRule class for an example.

template <class Rule>
class Universe
{
public:
	typedef typename Rule::State State;
	typedef typename Rule::StateBag StateBag;

	Universe(int WIDTH, int HEIGHT, const Rule& RULE)
		: rule(RULE), width(WIDTH), height(HEIGHT)
        , population(rule.make_state_bag())
	{
		grid_t* grids[] = { &grid_1, &grid_2 };
		for (int g = 0; g < 2; g++) {
			grids[g]->resize(width);
			for (int x = 0; x < width; x++) {
				(*grids[g])[x].resize(height);
			}
		}
		grid_front = &grid_1;
		grid_back = &grid_2;
	}

    virtual ~Universe() { }

	void clear() {
		for (int i = 0; i < width; i++) {
			for(int j = 0; j < height; j++) {
                population.remove((*grid_front)[i][j]);
				(*grid_front)[i][j] = State();
                population.add((*grid_front)[i][j]);
			}
		}
	}

	void set_random(float PERCENT, State VALUE) {
		for (int x = 1; x < width-1; x++) {
			for (int y = 1; y < height-1; y++) {
				if (randrange(0,1) < PERCENT) {
                    population.remove((*grid_front)[x][y]);
					(*grid_front)[x][y] = VALUE;
                    population.add((*grid_front)[x][y]);
				}
			}
		}
	}

	void integrate() {
        StateBag oldpop = population;
		for (int x = 1; x < width-1; x++) {
			for (int y = 1; y < height-1; y++) {
				StateBag count = rule.make_state_bag();
				grid_t& G = *grid_front;
				count.add(G[x-1][y-1]);
				count.add(G[x-1][y  ]);
				count.add(G[x-1][y+1]);
				count.add(G[x  ][y-1]);
			//	count.add(G[x  ][y  ]);   // we don't count the square itself
				count.add(G[x  ][y+1]);
				count.add(G[x+1][y-1]);
				count.add(G[x+1][y  ]);
				count.add(G[x+1][y+1]);

                population.remove(G[x][y]);
				(*grid_back)[x][y] = rule.prune(rule.next_state(G[x][y], count), oldpop);
                population.add((*grid_back)[x][y]);
			}
		}

		grid_t* temp = grid_front;
		grid_front = grid_back;
		grid_back = temp;
	}

	bool in_bounds(vec2 p) const {
		return 1 <= p.x && p.x < width-1 && 1 <= p.y && p.y < height-1;
	}

	void draw() const {
		float pointSize = VIEWSTACK.top().point_size();
		glPointSize(pointSize);
		glPushMatrix();
		glTranslatef(0.5, 0.5, 0);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				rule.draw(x, y, (*grid_front)[x][y]);
			}
		}
		glPopMatrix();
	}

	void set_at(vec2 POS, State VALUE) {
		if (!in_bounds(POS)) return ;
		int x = int(POS.x);
		int y = int(POS.y);
        population.remove((*grid_front)[x][y]);
		(*grid_front)[x][y] = VALUE;
        population.add((*grid_front)[x][y]);
	}

	State get_state_at(vec2 POS) {
		int x = int(POS.x);
		int y = int(POS.y);
		return (*grid_front)[x][y];
	}

	Rule get_rule() const { 
		return rule;
	}

    void insert_pattern(ptr< Pattern<Rule> > pat, vec2 ll) {
        int width  = int(pat->get_size().x);
        int height = int(pat->get_size().y);

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                set_at(ll + vec2(i,j), pat->get_at(vec2(i,j)));
            }
        }
    }
	
	int get_cost(ptr< Pattern<Rule> > pat, vec2 ll) {
        int width  = int(pat->get_size().x);
        int height = int(pat->get_size().y);

		int count = 0;

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
				if (get_state_at(ll + vec2(i, j)) != pat->get_at(vec2(i, j))) {
					count += 1;
				}
            }
        }

		return count;
    }

	ptr< Pattern<Rule> > make_pattern(vec2 start, vec2 end) {
		if (start.x > end.x) std::swap(start.x, end.x);
		if (start.y > end.y) std::swap(start.y, end.y);
		vec2 size = vec2(end.x - start.x, end.y - start.y);
		ptr< Pattern<Rule> > newPattern = new Pattern<Rule>(size);

		for (int i = 0; i < size.x; i++) {
			for (int j = 0; j < size.y; j++) {
				newPattern->set_at(vec2(i, j), (*grid_front)[int(start.x)+i][int(start.y)+j]);
			}
		}

		return newPattern;
	}

	typename Rule::StateBag get_population() const {
		return population;
	}
	
protected:
	Universe() : width(0), height(0), population(rule.make_state_bag()) {
		DIE("You forgot to construct Universe somewhere!");
	}

	typedef vector< vector<State> > grid_t;
	grid_t grid_1;
	grid_t grid_2;
	grid_t* grid_front;
	grid_t* grid_back;
	Rule rule;

	const int width, height;
    
    typename Rule::StateBag population;
};

#endif
