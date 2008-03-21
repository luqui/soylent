#include "Archivable.h"

ostream& operator<<(ostream &aOs, const Archivable& aA)
{
	aA.ToOs(aOs);
	return aOs;
}

istream& operator>>(istream &aIs, Archivable& aA)
{
	aA.FromIs(aIs);
	return aIs;
}

