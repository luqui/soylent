#ifndef __SOYLIB_PTR_H__
#define __SOYLIB_PTR_H__

template <class Ty>
class ptr
{
	template <class Tz> friend class ptr;

public:
	ptr() : ref_(0), refct_(new int(1)) { }  // XXX, should 0 have a refct?
	
	ptr(Ty* in) : ref_(in), refct_(new int(1)) { }

	ptr(const ptr& in) : ref_(in.ref_), refct_(in.refct_) {
		++*refct_;
	}

	template <class Tz>
	ptr(const ptr<Tz>& in) : ref_(in.ref_), refct_(in.refct_) {
		++*refct_;
	}

	~ptr() { release(); }

	Ty& operator * () const { return *ref_; }

	Ty* operator -> () const { return ref_; }

	ptr& operator = (const ptr& in) {
		if (ref_ == in.ref_) return *this;
		release();
		ref_ = in.ref_;
		refct_ = in.refct_;
		++*refct_;
		return *this;
	}

	template <class Tz>
	ptr& operator = (const ptr<Tz>& in) {
		if (ref_ == in.ref_) return;
		release();
		ref_ = in.ref_;
		refct_ = in.refct_;
		++*refct_;
	}

	bool null() const {
		return ref_ == 0;
	}

	bool operator!() const
	{
		return ref_ == 0;
	}

	bool operator&&(bool aB) const
	{
		return ref_ != 0 && aB;
	}

	bool valid() const {
		return ref_ != 0;
	}

	template <class Tz>
	bool operator == (Tz* other) const {
		return ref_ == other;
	}
	
	template <class Tz>
	bool operator == (ptr<Tz> other) const {
		return ref_ == other.ref_;
	}
	
	template <class Tz>
	bool operator != (Tz* other) const {
		return ref_ != other;
	}
	
	template <class Tz>
	bool operator != (ptr<Tz> other) const {
		return ref_ != other.ref_;
	}
	
private:
	void release() const {
		if (!--*refct_) {
			delete refct_;
			delete ref_;
		}
	}

	Ty* ref_;
	int* refct_;
};

template <class Ty>
bool operator&&(bool aB, const ptr<Ty> &aP)
{
	return aP && aB;
}

#endif
