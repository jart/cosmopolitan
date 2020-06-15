#define _GNU_SOURCE
#include "libc/math/math.h"

double significand(double x)
{
	return scalbn(x, -ilogb(x));
}
