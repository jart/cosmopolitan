#include "third_party/chibicc/test/test.h"

extern int ext1;
extern int *ext2;

inline int inline_fn(void) {
  return 3;
}

int main() {
  ASSERT(5, ext1);
  ASSERT(5, *ext2);

  extern int ext3;
  ASSERT(7, ext3);

  int ext_fn1(int x);
  ASSERT(5, ext_fn1(5));

  extern int ext_fn2(int x);
  ASSERT(8, ext_fn2(8));

  return 0;
}
