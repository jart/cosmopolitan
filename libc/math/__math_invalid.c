#include "libc/math/libm.h"

double __math_invalid(double x)
{
	return (x - x) / (x - x);
}
