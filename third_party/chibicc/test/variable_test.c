#include "third_party/chibicc/test/test.h"

int g1, g2[4];
static int g3 = 3;

int main() {
  ASSERT(3, ({
           int a;
           a = 3;
           a;
         }));
  ASSERT(3, ({
           int a = 3;
           a;
         }));
  ASSERT(8, ({
           int a = 3;
           int z = 5;
           a + z;
         }));

  ASSERT(3, ({
           int a = 3;
           a;
         }));
  ASSERT(8, ({
           int a = 3;
           int z = 5;
           a + z;
         }));
  ASSERT(6, ({
           int a;
           int b;
           a = b = 3;
           a + b;
         }));
  ASSERT(3, ({
           int foo = 3;
           foo;
         }));
  ASSERT(8, ({
           int foo123 = 3;
           int bar = 5;
           foo123 + bar;
         }));

  ASSERT(4, ({
           int x;
           sizeof(x);
         }));
  ASSERT(4, ({
           int x;
           sizeof x;
         }));
  ASSERT(8, ({
           int *x;
           sizeof(x);
         }));
  ASSERT(16, ({
           int x[4];
           sizeof(x);
         }));
  ASSERT(48, ({
           int x[3][4];
           sizeof(x);
         }));
  ASSERT(16, ({
           int x[3][4];
           sizeof(*x);
         }));
  ASSERT(4, ({
           int x[3][4];
           sizeof(**x);
         }));
  ASSERT(5, ({
           int x[3][4];
           sizeof(**x) + 1;
         }));
  ASSERT(5, ({
           int x[3][4];
           sizeof **x + 1;
         }));
  ASSERT(4, ({
           int x[3][4];
           sizeof(**x + 1);
         }));
  ASSERT(4, ({
           int x = 1;
           sizeof(x = 2);
         }));
  ASSERT(1, ({
           int x = 1;
           sizeof(x = 2);
           x;
         }));

  ASSERT(0, g1);
  ASSERT(3, ({
           g1 = 3;
           g1;
         }));
  ASSERT(0, ({
           g2[0] = 0;
           g2[1] = 1;
           g2[2] = 2;
           g2[3] = 3;
           g2[0];
         }));
  ASSERT(1, ({
           g2[0] = 0;
           g2[1] = 1;
           g2[2] = 2;
           g2[3] = 3;
           g2[1];
         }));
  ASSERT(2, ({
           g2[0] = 0;
           g2[1] = 1;
           g2[2] = 2;
           g2[3] = 3;
           g2[2];
         }));
  ASSERT(3, ({
           g2[0] = 0;
           g2[1] = 1;
           g2[2] = 2;
           g2[3] = 3;
           g2[3];
         }));

  ASSERT(4, sizeof(g1));
  ASSERT(16, sizeof(g2));

  ASSERT(1, ({
           char x = 1;
           x;
         }));
  ASSERT(1, ({
           char x = 1;
           char y = 2;
           x;
         }));
  ASSERT(2, ({
           char x = 1;
           char y = 2;
           y;
         }));

  ASSERT(1, ({
           char x;
           sizeof(x);
         }));
  ASSERT(10, ({
           char x[10];
           sizeof(x);
         }));

  ASSERT(2, ({
           int x = 2;
           { int x = 3; }
           x;
         }));
  ASSERT(2, ({
           int x = 2;
           { int x = 3; }
           int y = 4;
           x;
         }));
  ASSERT(3, ({
           int x = 2;
           { x = 3; }
           x;
         }));

  ASSERT(7, ({
           int x;
           int y;
           char z;
           char *a = &y;
           char *b = &z;
           b - a;
         }));
  ASSERT(1, ({
           int x;
           char y;
           int z;
           char *a = &y;
           char *b = &z;
           b - a;
         }));

  ASSERT(8, ({
           long x;
           sizeof(x);
         }));
  ASSERT(2, ({
           short x;
           sizeof(x);
         }));

  ASSERT(24, ({
           char *x[3];
           sizeof(x);
         }));
  ASSERT(8, ({
           char(*x)[3];
           sizeof(x);
         }));
  ASSERT(1, ({
           char(x);
           sizeof(x);
         }));
  ASSERT(3, ({
           char(x)[3];
           sizeof(x);
         }));
  ASSERT(12, ({
           char(x[3])[4];
           sizeof(x);
         }));
  ASSERT(4, ({
           char(x[3])[4];
           sizeof(x[0]);
         }));
  ASSERT(3, ({
           char *x[3];
           char y;
           x[0] = &y;
           y = 3;
           x[0][0];
         }));
  ASSERT(4, ({
           char x[3];
           char(*y)[3] = x;
           y[0][0] = 4;
           y[0][0];
         }));

  { void *x; }

  ASSERT(3, g3);

  return 0;
}
