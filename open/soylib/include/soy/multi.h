#ifndef __SOYLIB_MULTI_H__
#define __SOYLIB_MULTI_H__

namespace multimethod
{

template <class D, class B>
class IsDerivedFrom {
    typedef a[1] Yes;
    typedef a[2] No;

    static Yes Test(B*);
    static No Test(...);
    
public:
    static const bool value 
        = sizeof(Test(static_cast<D*>(0))) == sizeof(Yes) ? true : false;
};





template <class Base>
class Class {
public:
    virtual ~Class() { }

    virtual bool match(const Base* b) const = 0;
    virtual bool subset(const Class* m) const = 0;
};

};

#endif
