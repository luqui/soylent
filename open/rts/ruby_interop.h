#ifndef __RUBY_INTEROP_H__
#define __RUBY_INTEROP_H__

#include <ruby.h>
#include <soy/vec2.h>
#include <cstdlib>
#include <string>

inline VALUE rbw_vec2_to_value(vec2 in) {
	return rb_funcall(rb_path2class("Vec"), rb_intern("new"), 2, 
	                  rb_float_new(in.x), rb_float_new(in.y));
}

inline vec2 rbw_value_to_vec2(VALUE in) {
	return vec2(NUM2DBL(rb_funcall(in, rb_intern("x"), 0)),
	            NUM2DBL(rb_funcall(in, rb_intern("y"), 0)));
}

template <class Iter, class Conv>
VALUE rbw_seq_to_value(Iter begin, Iter end, Conv conv) {
	VALUE ret = rb_ary_new();
	for (Iter i = begin; i != end; ++i) {
		rb_ary_push(ret, conv(*i));
	}
	return ret;
}

template <class Ty>
void _rbw_typed_free(Ty** ptr) {
	delete *ptr;
	free(ptr);
}

// *** Careful!  All the unit functions expect a Unit*, not any subclasses 
// *** (apparently it does matter).  To be safe, always call this function 
// *** as rbw_ptr_to_value<Unit>(...) if you are dealing with units.
template <class Ty>
VALUE rbw_ptr_to_value(Ty* ptr, std::string type) {
	Ty** tmp;
	VALUE ret = Data_Make_Struct(rb_path2class(type.c_str()), Ty*, NULL, free, tmp);
	*tmp = ptr;
	return ret;
}

template <class Ty>
VALUE rbw_gc_ptr_to_value(Ty* ptr, std::string type) {
	Ty** tmp;
	VALUE ret = Data_Make_Struct(rb_path2class(type.c_str()), Ty*, NULL, 
			reinterpret_cast<void(*)(void*)>((void(*)(Ty**))&_rbw_typed_free), tmp);
	*tmp = ptr;
	return ret;
}

// *** Again, always get units back as rbw_value_to_ptr<Unit>(...)
template <class Ty>
Ty* rbw_value_to_ptr(VALUE in) {
	Ty** ret;
	Data_Get_Struct(in, Ty*, ret);
	return *ret;
}


void setup_ruby_wrappers();

void rbw_deliver_message(VALUE target, VALUE msg);

#endif
