#ifndef __GENERATIONALRULE_H__
#define __GENERATIONALRULE_H__

#include "config.h"
#include <GL/gl.h>
#include <GL/glu.h>

class GenerationalRule
{
public:
	typedef int State;
	
	struct StateBag {
		StateBag(const GenerationalRule* r) : rule(r), count(0) { }
		void add(State s) { if (s == rule->C) count++; }
		const GenerationalRule* rule;
		int count;
	};
	
	const int S, B, C;
	
	GenerationalRule(int S, int B, int C)
		: S(S), B(B), C(C)
	{ }

	StateBag make_state_bag() const {
		return StateBag(this);
	}

	State toggle(State state) const {
		if(state == C) {//alive
			return 0;
		} else {
			return C;
		}
	}

	State next_state(State STATE, StateBag COUNT) const {
		int bit = 1 << COUNT.count;
		if (B & bit && STATE == 0) return C;  // born
		if (S & bit && STATE == C) return C;  // survive
		return STATE ? STATE-1 : 0;			  // die
	}
	
	void draw(float X, float Y, State STATE) const {
		float color = float(STATE)/C;
		glColor4f(color, color, color, ALPHA_FACTOR);
		glBegin(GL_POINTS);
		glVertex2f(X,Y);
		glEnd();
	}

	State alive_state() const { return C; }

	State dead_state() const { return 0; }
};

#endif
