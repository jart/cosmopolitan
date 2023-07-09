#include "third_party/chibicc/test/test.h"

typedef int MyInt, MyInt2[4];
typedef int;

int main() {
  ASSERT(1, ({
           typedef int t;
           t x = 1;
           x;
         }));
  ASSERT(1, ({
           typedef struct {
             int a;
           } t;
           t x;
           x.a = 1;
           x.a;
         }));
  ASSERT(1, ({
           typedef int t;
           t t = 1;
           t;
         }));
  ASSERT(2, ({
           typedef struct {
             int a;
           } t;
           { typedef int t; }
           t x;
           x.a = 2;
           x.a;
         }));
  ASSERT(8, ({  // [jart] implicit long
           typedef t;
           t x;
           sizeof(x);
         }));
  ASSERT(3, ({
           MyInt x = 3;
           x;
         }));
  ASSERT(16, ({
           MyInt2 x;
           sizeof(x);
         }));

  return 0;
}
