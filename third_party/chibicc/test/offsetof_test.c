#include "third_party/chibicc/test/test.h"

typedef struct {
  int a;
  char b;
  int c;
  double d;
} T;

int main() {
  ASSERT(0, offsetof(T, a));
  ASSERT(4, offsetof(T, b));
  ASSERT(8, offsetof(T, c));
  ASSERT(16, offsetof(T, d));

  return 0;
}
