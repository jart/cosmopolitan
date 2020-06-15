#include "libc/math/math.h"

double drem(double x, double y)
{
	int q;
	return remquo(x, y, &q);
}
