#include <fenv.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  long double x;

  /*
    test outliers
  */

  // Test parsing of LDBL_EPSILON - use architecture-specific string
#ifdef __x86_64__
  // x86_64: 80-bit extended precision
  x = strtold("1.08420217248550443400745280086994171e-19", 0);
#else
  // aarch64: 128-bit IEEE quad precision
  x = strtold("1.92592994438723585305597794258492732e-34", 0);
#endif
  if (x != LDBL_EPSILON) {
    printf("#1: got %.36Lg\n", x);
    exit(1);
  }

  // Test a simple case that should work on both architectures
#ifdef __x86_64__
  x = strtold("1.08420217248550443400745280086994171e-18", 0);
  if (x != 1.08420217248550443400745280086994171e-18) {
    printf("#4: got %.36Lg\n", x);
    exit(4);
  }
#endif

  // Test LDBL_MIN - should be the same on both architectures
  x = strtold("3.36210314311209350626267781732175260e-4932", 0);
  if (x != LDBL_MIN) {
    printf("#2: got %.36Lg\n", x);
    exit(2);
  }

  // Test LDBL_MAX - use architecture-specific string
#ifdef __x86_64__
  // x86_64 version
  x = strtold("1.18973149535723176502126385303097021e+4932", 0);
#else
  // aarch64 version
  x = strtold("1.18973149535723176508575932662800702e+4932", 0);
#endif
  if (x != LDBL_MAX) {
    printf("#3: got %.36Lg\n", x);
    exit(3);
  }

  /*
    test rounding
  */

#ifdef __x86_64__
  x = strtold("1.00000000000000000001", 0);
  if (x != 1) {
    printf("#5: got %.36Lg\n", x);
    exit(5);
  }
  fesetround(FE_UPWARD);
  x = strtold("1.00000000000000000001", 0);
  if (x != 1.0000000000000000001L) {
    printf("#6: got %.36Lg\n", x);
    exit(6);
  }
#else
  x = strtold("1.00000000000000000000000000000000001", 0);
  if (x != 1) {
    printf("#5: got %.36Lg\n", x);
    exit(5);
  }
  fesetround(FE_UPWARD);
  x = strtold("1.00000000000000000000000000000000001", 0);
  if (x != 1.0000000000000000000000000000000002L) {
    printf("#6: got %.36Lg\n", x);
    exit(6);
  }
#endif
}
