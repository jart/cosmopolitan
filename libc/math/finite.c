#define _GNU_SOURCE
#include "libc/math/math.h"

int finite(double x)
{
	return isfinite(x);
}
