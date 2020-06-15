#include "libc/limits.h"
#include "libc/math/math.h"

double scalbln(double x, long n) {
  if (n > INT_MAX)
    n = INT_MAX;
  else if (n < INT_MIN)
    n = INT_MIN;
  return scalbn(x, n);
}
