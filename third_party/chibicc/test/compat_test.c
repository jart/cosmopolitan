#include "third_party/chibicc/test/test.h"

_Noreturn noreturn_fn(int restrict x) {
  exit(0);
}

void funcy_type(int arg[restrict static 3]) {
}

int main() {
  { volatile x; }
  { int volatile x; }
  { volatile int x; }
  { volatile int volatile volatile x; }
  { int volatile *volatile volatile x; }
  { auto **restrict __restrict __restrict__ const volatile *x; }
  return 0;
}
