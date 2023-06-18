#include "third_party/chibicc/test/test.h"

int main() {

  ASSERT(0, 0);
  ASSERT(42, 42);
  ASSERT(21, 5 + 20 - 4);
  ASSERT(41, 12 + 34 - 5);
  ASSERT(47, 5 + 6 * 7);
  ASSERT(15, 5 * (9 - 6));
  ASSERT(4, (3 + 5) / 2);
  ASSERT(10, -10 + 20);

  ASSERT(0, 0 == 1);
  ASSERT(1, 42 == 42);
  ASSERT(1, 0 != 1);
  ASSERT(0, 42 != 42);

  ASSERT(1, 0 < 1);
  ASSERT(0, 1 < 1);
  ASSERT(0, 2 < 1);
  ASSERT(1, 0 <= 1);
  ASSERT(1, 1 <= 1);
  ASSERT(0, 2 <= 1);

  ASSERT(1, 1 > 0);
  ASSERT(0, 1 > 1);
  ASSERT(0, 1 > 2);
  ASSERT(1, 1 >= 0);
  ASSERT(1, 1 >= 1);
  ASSERT(0, 1 >= 2);

  ASSERT(0, 1073741824 * 100 / 100);

  ASSERT(7, ({
           int i = 2;
           i += 5;
           i;
         }));
  ASSERT(7, ({
           int i = 2;
           i += 5;
         }));
  ASSERT(3, ({
           int i = 5;
           i -= 2;
           i;
         }));
  ASSERT(3, ({
           int i = 5;
           i -= 2;
         }));
  ASSERT(6, ({
           int i = 3;
           i *= 2;
           i;
         }));
  ASSERT(6, ({
           int i = 3;
           i *= 2;
         }));
  ASSERT(3, ({
           int i = 6;
           i /= 2;
           i;
         }));
  ASSERT(3, ({
           int i = 6;
           i /= 2;
         }));

  ASSERT(3, ({
           int i = 2;
           ++i;
         }));
  ASSERT(2, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           ++*p;
         }));
  ASSERT(0, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           --*p;
         }));

  ASSERT(2, ({
           int i = 2;
           i++;
         }));
  ASSERT(2, ({
           int i = 2;
           i--;
         }));
  ASSERT(3, ({
           int i = 2;
           i++;
           i;
         }));
  ASSERT(1, ({
           int i = 2;
           i--;
           i;
         }));
  ASSERT(1, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           *p++;
         }));
  ASSERT(1, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           *p--;
         }));

  ASSERT(0, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p++)--;
           a[0];
         }));
  ASSERT(0, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*(p--))--;
           a[1];
         }));
  ASSERT(2, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p)--;
           a[2];
         }));
  ASSERT(2, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p)--;
           p++;
           *p;
         }));

  ASSERT(0, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p++)--;
           a[0];
         }));
  ASSERT(0, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p++)--;
           a[1];
         }));
  ASSERT(2, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p++)--;
           a[2];
         }));
  ASSERT(2, ({
           int a[3];
           a[0] = 0;
           a[1] = 1;
           a[2] = 2;
           int *p = a + 1;
           (*p++)--;
           *p;
         }));

  ASSERT(0, !1);
  ASSERT(0, !2);
  ASSERT(1, !0);
  ASSERT(1, !(char)0);
  ASSERT(0, !(long)3);
  ASSERT(4, sizeof(!(char)0));
  ASSERT(4, sizeof(!(long)0));

  ASSERT(-1, ~0);
  ASSERT(0, ~-1);

  ASSERT(5, 17 % 6);
  ASSERT(5, ((long)17) % 6);
  ASSERT(2, ({
           int i = 10;
           i %= 4;
           i;
         }));
  ASSERT(2, ({
           long i = 10;
           i %= 4;
           i;
         }));

  ASSERT(0, 0 & 1);
  ASSERT(1, 3 & 1);
  ASSERT(3, 7 & 3);
  ASSERT(10, -1 & 10);

  ASSERT(1, 0 | 1);
  ASSERT(0b10011, 0b10000 | 0b00011);

  ASSERT(0, 0 ^ 0);
  ASSERT(0, 0b1111 ^ 0b1111);
  ASSERT(0b110100, 0b111000 ^ 0b001100);

  ASSERT(2, ({
           int i = 6;
           i &= 3;
           i;
         }));
  ASSERT(7, ({
           int i = 6;
           i |= 3;
           i;
         }));
  ASSERT(10, ({
           int i = 15;
           i ^= 5;
           i;
         }));

  ASSERT(1, 1 << 0);
  ASSERT(8, 1 << 3);
  ASSERT(10, 5 << 1);
  ASSERT(2, 5 >> 1);
  ASSERT(-1, -1 >> 1);
  ASSERT(1, ({
           int i = 1;
           i <<= 0;
           i;
         }));
  ASSERT(8, ({
           int i = 1;
           i <<= 3;
           i;
         }));
  ASSERT(10, ({
           int i = 5;
           i <<= 1;
           i;
         }));
  ASSERT(2, ({
           int i = 5;
           i >>= 1;
           i;
         }));
  ASSERT(-1, -1);
  ASSERT(-1, ({
    int i = -1;
    i;
  }));
  ASSERT(-1, ({
    int i = -1;
    i >>= 1;
    i;
  }));

  ASSERT(2, 0 ? 1 : 2);
  ASSERT(1, 1 ? 1 : 2);
  ASSERT(-1, 0 ? -2 : -1);
  ASSERT(-2, 1 ? -2 : -1);
  ASSERT(4, sizeof(0 ? 1 : 2));
  ASSERT(8, sizeof(0 ? (long)1 : (long)2));
  ASSERT(-1, 0 ? (long)-2 : -1);
  ASSERT(-1, 0 ? -2 : (long)-1);
  ASSERT(-2, 1 ? (long)-2 : -1);
  ASSERT(-2, 1 ? -2 : (long)-1);

  1 ? -2 : (void)-1;

  ASSERT(20, ({
           int x;
           int *p = &x;
           p + 20 - p;
         }));
  ASSERT(1, ({
           int x;
           int *p = &x;
           p + 20 - p > 0;
         }));
  ASSERT(-20, ({
    int x;
    int *p = &x;
    p - 20 - p;
  }));
  ASSERT(1, ({
           int x;
           int *p = &x;
           p - 20 - p < 0;
         }));

  ASSERT(15, (char *)0xffffffffffffffff - (char *)0xfffffffffffffff0);
  ASSERT(-15, (char *)0xfffffffffffffff0 - (char *)0xffffffffffffffff);
  ASSERT(1, (void *)0xffffffffffffffff > (void *)0);

  ASSERT(3, 3 ?: 5);
  ASSERT(5, 0 ?: 5);
  ASSERT(4, ({
           int i = 3;
           ++i ?: 10;
         }));

  ASSERT(3, (long double)3);
  ASSERT(5, (long double)3 + 2);
  ASSERT(6, (long double)3 * 2);
  ASSERT(5, (long double)3 + 2.0);

  return 0;
}
