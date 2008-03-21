#ifndef __MOUSEINTERPRETER_H__
#define __MOUSEINTERPRETER_H__

#include "config.h"

const int POS_BUF_SIZE = 3;

class MouseInterpreter
{
public:
	MouseInterpreter(vec2 posin) {
		for (int i = 0; i < POS_BUF_SIZE; i++) {
			last_pos_[i] = posin;
		}
	}
	
	void step() {
		velocity_ = (MOUSE - last_pos_[0]) / (DT*POS_BUF_SIZE);
		for (int i = 0; i < POS_BUF_SIZE-1; i++) {
			last_pos_[i] = last_pos_[i+1];
		}
		last_pos_[POS_BUF_SIZE-1] = MOUSE;
	}

	float jumps() const {
		if (fabs(velocity_.y) > 20) {
			float ret = velocity_.y / 40;
			if (ret > 2) ret = 2;
			return ret;
		}
		else {
			return 0;
		}
	}

	float flies() const {
		float ret = (MOUSE.y - DUDE->pos().y)/5;
		if (ret > 1) return 1;
		if (ret < -1) return -1;
		return ret;
	}

	float walks() const {
		float ret = (MOUSE.x - DUDE->pos().x)/5;
		if (ret > 1) return 1;
		if (ret < -1) return -1;
		return ret;
	}
	
private:
	vec2 last_pos_[POS_BUF_SIZE];
	vec2 velocity_;
};

#endif
