#ifndef __SOYLIB_WEAK_H__
#define __SOYLIB_WEAK_H__

#include "soy/error.h"
#include <cassert>
#include <vector>
#include <map>

class _weak_pool
{
protected:
	typedef unsigned int age_t;
	typedef unsigned int idx_t;

	static void init_range(size_t lo, size_t hi) {
		for (size_t i = lo; i < hi; i++) {
			pool_[i].age = 0;
			pool_[i].next = i+1;
		}
		head_ = lo;
	}
	
	static idx_t allocate(void* ptr) {
		indices_t::const_iterator i = indices_.find(ptr);
		if (i != indices_.end()) {
			return i->second;
		}
		
		if (alloced_ >= pool_.size()) {
			size_t sz = pool_.size();
			size_t newsz = sz == 0 ? 64 : 2*sz;
			pool_.resize(newsz);
			init_range(sz, newsz);
		}

		idx_t ret = head_;
		head_ = pool_[ret].next;
		assert(ret < pool_.size());
		pool_[ret].ptr = ptr;
		indices_[ptr] = ret;
		return ret;
	}
	
	static void* fetch(idx_t idx) {
		return pool_[idx].ptr;
	}
	
	static age_t get_age(idx_t idx) { 
		return pool_[idx].age;
	}

	static void deallocate(idx_t idx) {
		indices_.erase(fetch(idx));
		++pool_[idx].age;
		if (pool_[idx].age == age_t(~0)) {  // we have exhausted the age counter
			pool_[idx].next = idx_t(~0);
		}
		else {
			pool_[idx].next = head_;
			head_ = idx;
		}
	}
private:

	struct Entry {
		age_t age;
		union {
			idx_t next;
			void* ptr;
		};
	};

	typedef std::vector<Entry> pool_t;
	typedef std::map<void*, idx_t> indices_t;
	static pool_t pool_;
	static indices_t indices_;
	static idx_t head_;
	static size_t alloced_;
};

template <class Tx>
class weak : private _weak_pool
{
	template <class Ty> friend class weak;
public:
	weak(Tx* in = NULL) {
		if (in == NULL) { 
			idx_ = idx_t(~0); 
			age_ = 0; 
		}
		else { 
			idx_ = allocate(in);
			age_ = get_age(idx_); 
		}
	}
	
	weak(const weak& in) : idx_(in.idx_), age_(in.age_)     { }

	template <class Ty>
	weak(const weak<Ty>& in) : idx_(in.idx_), age_(in.age_) { }

	weak& operator = (const weak& in) {
		idx_ = in.idx_;
		age_ = in.age_;
		return *this;
	}
	
	template <class Ty>
	weak& operator = (const weak<Ty>& in) {
		idx_ = in.idx_;
		age_ = in.age_;
		return *this;
	}

	Tx& operator * () const {
		if (valid()) {
			return *static_cast<Tx*>(fetch(idx_));
		}
		else {
			abort();
		}
	}

	Tx* operator -> () const {
		if (valid()) {
			return static_cast<Tx*>(fetch(idx_));
		}
		else {
			abort();
		}
	}

	void destroy() {
		if (valid()) {
			delete static_cast<Tx*>(fetch(idx_));
			deallocate(idx_);
		}
		// deleting an already destroyed pointer is no error
	}

	bool valid() const {
		return idx_ != idx_t(~0) && age_ == get_age(idx_);
	}

	template <class Ty>
	bool operator == (const weak<Ty>& in) const {
		return idx_ == in.idx_ && age_ == in.age_;
	}

	template <class Ty>
	bool operator == (Ty* in) const {
		return valid() ? in == fetch(idx_) : in == 0;
	}

	template <class Ty>
	bool operator != (Ty in) const {
		return !(*this == in);
	}

	template <class Ty>
	weak<Ty> as() const {
		return weak<Ty>(dynamic_cast<Ty*>(static_cast<Tx*>(fetch(idx_))));
	}
	
private:
	idx_t idx_;
	age_t age_;
};

#endif
