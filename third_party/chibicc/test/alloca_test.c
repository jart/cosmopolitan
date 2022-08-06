#include "libc/mem/alloca.h"
#include "third_party/chibicc/test/test.h"

void *fn(int x, void *p, int y) {
  return p;
}

int main() {
  int i = 0;

  char *p1 = alloca(16);
  char *p2 = alloca(16);
  char *p3 = 1 + (char *)alloca(3) + 1;
  p3 -= 2;
  char *p4 = fn(1, __builtin_alloca(16), 3);

  ASSERT(16, p1 - p2);
  ASSERT(16, p2 - p3);
  ASSERT(16, p3 - p4);

  memcpy(p1, "0123456789abcdef", 16);
  memcpy(p2, "ghijklmnopqrstuv", 16);
  memcpy(p3, "wxy", 3);

  ASSERT(0, memcmp(p1, "0123456789abcdef", 16));
  ASSERT(0, memcmp(p2, "ghijklmnopqrstuv", 16));
  ASSERT(0, memcmp(p3, "wxy", 3));

  return 0;
}
