#ifndef Archivable_h
#define Archivable_h

/*
// To inherit from Archivable, paste this into your object declaration:

	virtual void ToOs(ostream &aOs) const;
	virtual void FromIs(istream &aIs);

// These can go in your .cpp file.

void NewObj::ToOs(ostream &aOs) const
{
}

void NewObj::FromIs(istream &aIs)
{
}
*/

// #include "Port.h"
#include <iostream>

using namespace std;

class Archivable
{
public:
	virtual void ToOs(ostream &aos) const = 0;
	virtual void FromIs(istream &aos) = 0;
};

ostream& operator<<(ostream &aOs, const Archivable& aA);
istream& operator>>(istream &aIs, Archivable& aA);

#endif
