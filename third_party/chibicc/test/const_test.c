#include "third_party/chibicc/test/test.h"

int main() {
  { const x; }
  { int const x; }
  { const int x; }
  { const int const const x; }
  ASSERT(5, ({
           const x = 5;
           x;
         }));
  ASSERT(8, ({
           const x = 8;
           int *const y = &x;
           *y;
         }));
  ASSERT(6, ({
           const x = 6;
           *(const *const) & x;
         }));

  return 0;
}
