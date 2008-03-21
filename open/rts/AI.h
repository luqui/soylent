#ifndef __AI_H__
#define __AI_H__

#include <ruby.h>

class AI
{
public:
	AI(VALUE ai = Qnil) : ai_(ai) {
		rb_gc_register_address(&ai_);
	}

	~AI() {
		if (!NIL_P(ai_)) {
			rb_funcall(ai_, rb_intern("disconnect"), 0);
		}
		rb_gc_unregister_address(&ai_);
	}

	void set(VALUE in) {
		if (in != ai_ && !NIL_P(ai_)) {
			rb_funcall(ai_, rb_intern("disconnect"), 0);
		}
		ai_ = in;
	}

	VALUE get() const {
		return ai_;
	}

	void step() const {
		if (!NIL_P(ai_)) {
			rb_funcall(ai_, rb_intern("step"), 0);
		}
	}
private:
	VALUE ai_;
};

#endif
