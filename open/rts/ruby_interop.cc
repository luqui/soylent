#include "ruby_interop.h"
#include "Unit.h"
#include "PathFind.h"
#include <new>
#include <exception>

void* operator new (size_t size) {
	return ALLOC_N(char, size);
}

void operator delete (void* ptr) {
	free(ptr);
}


#define RBW_DEFINE_0(name, type, code) \
	static VALUE name (VALUE _rbself) { \
		type* _rself; \
		Data_Get_Struct(_rbself, type, _rself); \
		type& self = *_rself; \
		code \
	}

#define RBW_DEFINE_1(name, type, arg, code) \
	static VALUE name (VALUE _rbself, VALUE arg) { \
		type* _rself; \
		Data_Get_Struct(_rbself, type, _rself); \
		type& self = *_rself; \
		code \
	}

#define RBW_DEFINE_CAST_0(name, base, type, code) \
	RBW_DEFINE_0(name, base, { \
			type _tmpself = dynamic_cast<type>(self); \
			if (!_tmpself) DIE("Bad cast (" #name "): not a " #type); \
			type& self = _tmpself; \
			code \
	})

#define RBW_DEFINE_CAST_1(name, base, type, arg, code) \
	RBW_DEFINE_1(name, base, arg, { \
			type _tmpself = dynamic_cast<type>(self); \
			if (!_tmpself) DIE("Bad cast (" #name "): not a " #type); \
			type& self = _tmpself; \
			code \
	})

#define RBW_FUNC(p) reinterpret_cast<VALUE (*)(...)>(p)

// ** Unit **

RBW_DEFINE_0(rbw_Unit_draw, Unit*, {
		self->draw();
		return Qnil;
});

RBW_DEFINE_0(rbw_Unit_get_position, Unit*, {
		vec2 ret = self->get_position();
		return rbw_vec2_to_value(ret);
});

RBW_DEFINE_1(rbw_Unit_set_ai, Unit*, ai, {
		self->set_ai(ai);
		return Qnil;
});

// XXX TODO bounding_box

RBW_DEFINE_0(rbw_Unit_step, Unit*, {
		self->step();
		return Qnil;
});

static void rbw_setup_Unit() {
	VALUE cls = rb_define_class("Unit", rb_cObject);
	rb_define_method(cls, "draw",            RBW_FUNC(rbw_Unit_draw), 0);
	rb_define_method(cls, "get_position",    RBW_FUNC(rbw_Unit_get_position), 0);
	rb_define_method(cls, "set_ai",          RBW_FUNC(rbw_Unit_set_ai), 1);
	rb_define_method(cls, "step",            RBW_FUNC(rbw_Unit_step), 0);
}

// ** MobileUnit **

RBW_DEFINE_CAST_1(rbw_MobileUnit_set_velocity, Unit*, MobileUnit*, vel, {
		self->set_velocity(rbw_value_to_vec2(vel));
		return Qnil;
});

RBW_DEFINE_CAST_0(rbw_MobileUnit_get_velocity, Unit*, MobileUnit*, {
		return rbw_vec2_to_value(self->get_velocity());
});

RBW_DEFINE_CAST_0(rbw_MobileUnit_get_speed_aptitude, Unit*, MobileUnit*, {
		UnitSpeedAptitude apt = self->get_speed_aptitude();
		UnitSpeedAptitude* clone = new UnitSpeedAptitude(apt);
		return rbw_gc_ptr_to_value<UnitSpeedAptitude>(clone, "UnitSpeedAptitude");
});

RBW_DEFINE_CAST_0(rbw_MobileUnit_obstructed, Unit*, MobileUnit*, {
		return self->obstructed() ? Qtrue : Qfalse;
});

RBW_DEFINE_CAST_1(rbw_MobileUnit_nudge, Unit*, MobileUnit*, dp, {
		return rbw_vec2_to_value(self->nudge(rbw_value_to_vec2(dp)));
});

RBW_DEFINE_CAST_0(rbw_MobileUnit_get_angle, Unit*, MobileUnit*, {
		return rb_float_new(self->get_angle());
});

static void rbw_setup_MobileUnit() {
	VALUE cls = rb_define_class("MobileUnit", rb_path2class("Unit"));
	rb_define_method(cls, "set_velocity",  RBW_FUNC(rbw_MobileUnit_set_velocity), 1);
	rb_define_method(cls, "get_velocity",  RBW_FUNC(rbw_MobileUnit_get_velocity), 0);
	rb_define_method(cls, "get_speed_aptitude",  RBW_FUNC(rbw_MobileUnit_get_speed_aptitude), 0);
	rb_define_method(cls, "obstructed",    RBW_FUNC(rbw_MobileUnit_obstructed), 0);
	rb_define_method(cls, "nudge",         RBW_FUNC(rbw_MobileUnit_nudge), 1);
	rb_define_method(cls, "get_angle",     RBW_FUNC(rbw_MobileUnit_get_angle), 1);
}

// ** UnitSpeedAptitude **

RBW_DEFINE_0(rbw_UnitSpeedAptitude_get_max_speed, UnitSpeedAptitude*, {
		return rb_float_new(self->get_max_speed());
});

RBW_DEFINE_0(rbw_UnitSpeedAptitude_get_acceleration, UnitSpeedAptitude*, {
		return rb_float_new(self->get_acceleration());
});

RBW_DEFINE_0(rbw_UnitSpeedAptitude_get_deceleration, UnitSpeedAptitude*, {
		return rb_float_new(self->get_deceleration());
});

RBW_DEFINE_0(rbw_UnitSpeedAptitude_get_turn_rate, UnitSpeedAptitude*, {
		return rb_float_new(self->get_turn_rate());
});

static void rbw_setup_UnitSpeedAptitude() {
	VALUE cls = rb_define_class("UnitSpeedAptitude", rb_cObject);
	rb_define_method(cls, "get_max_speed", RBW_FUNC(rbw_UnitSpeedAptitude_get_max_speed), 0);
	rb_define_method(cls, "get_acceleration", RBW_FUNC(rbw_UnitSpeedAptitude_get_acceleration), 0);
	rb_define_method(cls, "get_deceleration", RBW_FUNC(rbw_UnitSpeedAptitude_get_deceleration), 0);
	rb_define_method(cls, "get_turn_rate", RBW_FUNC(rbw_UnitSpeedAptitude_get_turn_rate), 0);
}

// ** PathFind **

static VALUE rbw_PathFind_new(VALUE self) {
	PathFind* pf = new PathFind;
	VALUE pfv = rbw_gc_ptr_to_value<PathFind>(pf, "PathFind");
	rb_obj_call_init(pfv, 0, NULL);
	return pfv;
}

RBW_DEFINE_1(rbw_PathFind_set_dest, PathFind*, dest, {
		self->set_dest(rbw_value_to_vec2(dest));
		return Qnil;
});

RBW_DEFINE_1(rbw_PathFind_set_speed_apt, PathFind*, speed, {
		self->set_speed_apt(*rbw_value_to_ptr<UnitSpeedAptitude>(speed));
		return Qnil;
});

RBW_DEFINE_1(rbw_PathFind_get_weight, PathFind*, src, {
		return rb_float_new(self->get_weight(rbw_value_to_vec2(src)));
});

static void rbw_setup_PathFind() {
	VALUE cls = rb_define_class("PathFind", rb_cObject);
	rb_define_singleton_method(cls, "new", RBW_FUNC(rbw_PathFind_new), 0);
	rb_define_method(cls, "set_dest",      RBW_FUNC(rbw_PathFind_set_dest), 1);
	rb_define_method(cls, "set_speed_apt", RBW_FUNC(rbw_PathFind_set_speed_apt), 1);
	rb_define_method(cls, "get_weight",    RBW_FUNC(rbw_PathFind_get_weight), 1);
}

// ** Main Setup **

void setup_ruby_wrappers() {
	rbw_setup_Unit();
	rbw_setup_MobileUnit();
	rbw_setup_UnitSpeedAptitude();
	rbw_setup_PathFind();
}

// ** Other Stuff **

void rbw_deliver_message(VALUE target, VALUE msg)
{
	VALUE stream = rb_funcall(rb_path2class("PollDeliverStream"), rb_intern("new"), 2, target, msg);
	rb_funcall(stream, rb_intern("run"), 0);
}
