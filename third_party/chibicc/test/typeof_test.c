#include "third_party/chibicc/test/test.h"

int main() {
  ASSERT(3, ({
           typeof(int) x = 3;
           x;
         }));
  ASSERT(3, ({
           typeof(1) x = 3;
           x;
         }));
  ASSERT(4, ({
           int x;
           typeof(x) y;
           sizeof(y);
         }));
  ASSERT(8, ({
           int x;
           typeof(&x) y;
           sizeof(y);
         }));
  ASSERT(4, ({
           typeof("foo") x;
           sizeof(x);
         }));
  ASSERT(12, sizeof(typeof(struct { int a, b, c; })));
  ASSERT(3, ({
           label:
             3;
         }));
  ASSERT(3, ({
           __typeof(int) x = 3;
           x;
         }));
}
