#include "libc/math/math.h"
#include "libc/math/libm.h"

float lgammaf(float x)
{
	return __lgammaf_r(x, &__signgam);
}
