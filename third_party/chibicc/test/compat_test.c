#include "third_party/chibicc/test/test.h"

[[noreturn]] noreturn_fn1(int restrict x) {
  exit(0);
}

_Noreturn noreturn_fn2(int restrict x) {
  exit(0);
}

[[_Noreturn]] noreturn_fn3(int restrict x) {
  exit(0);
}

__attribute__((noreturn)) noreturn_fn4(int restrict x) {
  exit(0);
}

__attribute__((__noreturn__)) noreturn_fn5(int restrict x) {
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
