#include "libc/math/math.h"

long lroundl(long double x) {
  long y;
  x = roundl(x);
  y = x;
  return y;
}
