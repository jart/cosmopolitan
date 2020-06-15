#include "libc/math/math.h"
#include "libc/math/libm.h"

double lgamma(double x)
{
	return __lgamma_r(x, &__signgam);
}
