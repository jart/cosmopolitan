#define _GNU_SOURCE
#include "libc/math/math.h"

float significandf(float x)
{
	return scalbnf(x, -ilogbf(x));
}
