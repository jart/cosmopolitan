#include "libc/math/math.h"
#include "libc/str/str.h"

#if FLT_EVAL_METHOD == 0 || FLT_EVAL_METHOD == 1
#define EPS DBL_EPSILON
#elif FLT_EVAL_METHOD == 2
#define EPS LDBL_EPSILON
#endif

static const double_t toint = 1 / EPS;

double rint(double x) {
  int e, s;
  uint64_t i;
  double_t y;
  memcpy(&i, &x, 8);
  e = i >> 52 & 0x7ff;
  s = i >> 63;
  if (e >= 0x3ff + 52) {
    return x;
  }
  if (s) {
    y = x - toint + toint;
  } else {
    y = x + toint - toint;
  }
  if (y == 0) {
    return s ? -0.0 : 0;
  }
  return y;
}
