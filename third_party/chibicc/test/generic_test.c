#include "third_party/chibicc/test/test.h"

#warning ignore this c23 warning

int main() {
  ASSERT(1, _Generic(100.0, double : 1, int * : 2, int : 3, float : 4));
  ASSERT(2, _Generic((int *)0, double : 1, int * : 2, int : 3, float : 4));
  ASSERT(2, _Generic((int[3]){}, double : 1, int * : 2, int : 3, float : 4));
  ASSERT(3, _Generic(100, double : 1, int * : 2, int : 3, float : 4));
  ASSERT(4, _Generic(100f, double : 1, int * : 2, int : 3, float : 4));
  ASSERT(1, _Generic(true, bool: 1, default: 0));
  ASSERT(1, _Generic(false, bool: 1, default: 0));
  ASSERT(1, _Generic(nullptr, nullptr_t: 1, default: 0));
  return 0;
}
