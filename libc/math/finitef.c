#define _GNU_SOURCE
#include "libc/math/math.h"

int finitef(float x)
{
	return isfinite(x);
}
