#include "libc/math/math.h"

long lround(double x) {
  long y;
  x = round(x);
  y = x;
  return y;
}
