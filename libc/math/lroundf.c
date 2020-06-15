#include "libc/math/math.h"

long lroundf(float x) {
  long y;
  x = roundf(x);
  y = x;
  return y;
}
