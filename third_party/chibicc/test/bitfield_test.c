#include "third_party/chibicc/test/test.h"

#define TYPE_SIGNED(type) (((type)-1) < 0)

struct {
  char a;
  int b : 5;
  int c : 10;
} g45 = {1, 2, 3}, g46 = {};

int main() {

  /* NOTE: Consistent w/ GCC (but MSVC would fail this) */
  ASSERT(1, 2 == ({
              struct {
                enum { a, b, c } e : 2;
              } x = {
                  .e = 2,
              };
              x.e;
            }));

  /* NOTE: GCC forbids typeof(bitfield). */
  ASSERT(0, ({
           struct {
             enum { a, b, c } e : 2;
           } x = {
               .e = 2,
           };
           TYPE_SIGNED(typeof(x.e));
         }));

  ASSERT(4, sizeof(struct { int x : 1; }));
  ASSERT(8, sizeof(struct { long x : 1; }));

  struct bit1 {
    short a;
    char b;
    int c : 2;
    int d : 3;
    int e : 3;
  };

  ASSERT(4, sizeof(struct bit1));
  ASSERT(1, ({
           struct bit1 x;
           x.a = 1;
           x.b = 2;
           x.c = 3;
           x.d = 4;
           x.e = 5;
           x.a;
         }));
  ASSERT(1, ({
           struct bit1 x = {1, 2, 3, 4, 5};
           x.a;
         }));
  ASSERT(2, ({
           struct bit1 x = {1, 2, 3, 4, 5};
           x.b;
         }));
  ASSERT(-1, ({
    struct bit1 x = {1, 2, 3, 4, 5};
    x.c;
  }));
  ASSERT(-4, ({
    struct bit1 x = {1, 2, 3, 4, 5};
    x.d;
  }));
  ASSERT(-3, ({
    struct bit1 x = {1, 2, 3, 4, 5};
    x.e;
  }));

  ASSERT(1, g45.a);
  ASSERT(2, g45.b);
  ASSERT(3, g45.c);

  ASSERT(0, g46.a);
  ASSERT(0, g46.b);
  ASSERT(0, g46.c);

  typedef struct {
    int a : 10;
    int b : 10;
    int c : 10;
  } T3;

  ASSERT(1, ({
           T3 x = {1, 2, 3};
           x.a++;
         }));
  ASSERT(2, ({
           T3 x = {1, 2, 3};
           x.b++;
         }));
  ASSERT(3, ({
           T3 x = {1, 2, 3};
           x.c++;
         }));

  ASSERT(2, ({
           T3 x = {1, 2, 3};
           ++x.a;
         }));
  ASSERT(3, ({
           T3 x = {1, 2, 3};
           ++x.b;
         }));
  ASSERT(4, ({
           T3 x = {1, 2, 3};
           ++x.c;
         }));

  ASSERT(4, sizeof(struct {
           int a : 3;
           int c : 1;
           int c : 5;
         }));
  ASSERT(8, sizeof(struct {
           int a : 3;
           int : 0;
           int c : 5;
         }));
  ASSERT(4, sizeof(struct {
           int a : 3;
           int : 0;
         }));

  typedef struct {
    long p : 1;
    long a : 40;
    long b : 20;
  } T4;

  ASSERT(0xfffffffffffaaaaa, ({
           T4 x = {1, 0x31337313371337, 0xaaaaaaaaaaaaaaaa};
           x.b++;
         }));
  ASSERT(0x7313371337, ({
           T4 x = {1, 0x31337313371337, 0xaaaaaaaaaaaaaaaa};
           x.a++;
         }));
  ASSERT(0xffffffffffffffd4, ({
           T4 x;
           x.b = -44;
           x.b++;
         }));

  return 0;
}
