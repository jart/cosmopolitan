#include "libc/macros.h"
#include "third_party/chibicc/test/test.h"

int index1d(int xn, int p[xn], int x) {
  return p[x];
}

int index2d(int yn, int xn, int (*p)[yn][xn], int y, int x) {
  return (*p)[y][x];
}

int main() {

  ASSERT(30, ({
           int a[5] = {00, 10, 20, 30, 40, 50};
           index1d(5, a, 3);
         }));

  /* ASSERT(210, ({ */
  /*          int a[3][3] = { */
  /*              {000, 010, 020}, */
  /*              {100, 110, 120}, */
  /*              {200, 210, 220}, */
  /*          }; */
  /*          index2d(3, 3, a, 2, 1); */
  /*        })); */

  ASSERT(20, ({
           int n = 5;
           int x[n];
           sizeof(x);
         }));
  ASSERT(5, ({
           int n = 5;
           int x[n];
           ARRAYLEN(x);
         }));
  ASSERT((5 + 1) * (8 * 2) * 4, ({
           int m = 5, n = 8;
           int x[m + 1][n * 2];
           sizeof(x);
         }));

  ASSERT(8, ({
           char n = 10;
           int(*x)[n][n + 2];
           sizeof(x);
         }));
  ASSERT(480, ({
           char n = 10;
           int(*x)[n][n + 2];
           sizeof(*x);
         }));
  ASSERT(48, ({
           char n = 10;
           int(*x)[n][n + 2];
           sizeof(**x);
         }));
  ASSERT(4, ({
           char n = 10;
           int(*x)[n][n + 2];
           sizeof(***x);
         }));

  ASSERT(60, ({
           char n = 3;
           int x[5][n];
           sizeof(x);
         }));
  ASSERT(12, ({
           char n = 3;
           int x[5][n];
           sizeof(*x);
         }));

  ASSERT(60, ({
           char n = 3;
           int x[n][5];
           sizeof(x);
         }));
  ASSERT(20, ({
           char n = 3;
           int x[n][5];
           sizeof(*x);
         }));

  ASSERT(0, ({
           int n = 10;
           int x[n + 1][n + 6];
           int *p = x;
           for (int i = 0; i < sizeof(x) / 4; i++) p[i] = i;
           x[0][0];
         }));
  ASSERT(5, ({
           int n = 10;
           int x[n + 1][n + 6];
           int *p = x;
           for (int i = 0; i < sizeof(x) / 4; i++) p[i] = i;
           x[0][5];
         }));
  ASSERT(5 * 16 + 2, ({
           int n = 10;
           int x[n + 1][n + 6];
           int *p = x;
           for (int i = 0; i < sizeof(x) / 4; i++) p[i] = i;
           x[5][2];
         }));

  ASSERT(10, ({
           int n = 5;
           sizeof(char[2][n]);
         }));

  return 0;
}
