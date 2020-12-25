#include "third_party/chibicc/test/test.h"

char g3 = 3;
short g4 = 4;
int g5 = 5;
long g6 = 6;
int g9[3] = {0, 1, 2};
struct {
  char a;
  int b;
} g11[2] = {{1, 2}, {3, 4}};
struct {
  int a[2];
} g12[2] = {{{1, 2}}};
union {
  int a;
  char b[8];
} g13[2] = {0x01020304, 0x05060708};
char g17[] = "foobar";
char g18[10] = "foobar";
char g19[3] = "foobar";
char *g20 = g17 + 0;
char *g21 = g17 + 3;
char *g22 = &g17 - 3;
char *g23[] = {g17 + 0, g17 + 3, g17 - 3};
int g24 = 3;
int *g25 = &g24;
int g26[3] = {1, 2, 3};
int *g27 = g26 + 1;
int *g28 = &g11[1].a;
long g29 = (long)(long)g26;
struct {
  struct {
    int a[3];
  } a;
} g30 = {{{1, 2, 3}}};
int *g31 = g30.a.a;
struct {
  int a[2];
} g40[2] = {{1, 2}, 3, 4};
struct {
  int a[2];
} g41[2] = {1, 2, 3, 4};
char g43[][4] = {'f', 'o', 'o', 0, 'b', 'a', 'r', 0};
char *g44 = {"foo"};
union {
  int a;
  char b[4];
} g50 = {.b[2] = 0x12};
union {
  int a;
} g51[2] = {};

typedef char T60[];
T60 g60 = {1, 2, 3};
T60 g61 = {1, 2, 3, 4, 5, 6};

typedef struct {
  char a, b[];
} T65;
T65 g65 = {'f', 'o', 'o', 0};
T65 g66 = {'f', 'o', 'o', 'b', 'a', 'r', 0};

int main() {
  ASSERT(1, ({
           int x[3] = {1, 2, 3};
           x[0];
         }));
  ASSERT(2, ({
           int x[3] = {1, 2, 3};
           x[1];
         }));
  ASSERT(3, ({
           int x[3] = {1, 2, 3};
           x[2];
         }));
  ASSERT(3, ({
           int x[3] = {1, 2, 3};
           x[2];
         }));

  ASSERT(2, ({
           int x[2][3] = {{1, 2, 3}, {4, 5, 6}};
           x[0][1];
         }));
  ASSERT(4, ({
           int x[2][3] = {{1, 2, 3}, {4, 5, 6}};
           x[1][0];
         }));
  ASSERT(6, ({
           int x[2][3] = {{1, 2, 3}, {4, 5, 6}};
           x[1][2];
         }));

  ASSERT(0, ({
           int x[3] = {};
           x[0];
         }));
  ASSERT(0, ({
           int x[3] = {};
           x[1];
         }));
  ASSERT(0, ({
           int x[3] = {};
           x[2];
         }));

  ASSERT(2, ({
           int x[2][3] = {{1, 2}};
           x[0][1];
         }));
  ASSERT(0, ({
           int x[2][3] = {{1, 2}};
           x[1][0];
         }));
  ASSERT(0, ({
           int x[2][3] = {{1, 2}};
           x[1][2];
         }));

  ASSERT('a', ({
           char x[4] = "abc";
           x[0];
         }));
  ASSERT('c', ({
           char x[4] = "abc";
           x[2];
         }));
  ASSERT(0, ({
           char x[4] = "abc";
           x[3];
         }));
  ASSERT('a', ({
           char x[2][4] = {"abc", "def"};
           x[0][0];
         }));
  ASSERT(0, ({
           char x[2][4] = {"abc", "def"};
           x[0][3];
         }));
  ASSERT('d', ({
           char x[2][4] = {"abc", "def"};
           x[1][0];
         }));
  ASSERT('f', ({
           char x[2][4] = {"abc", "def"};
           x[1][2];
         }));

  ASSERT(4, ({
           int x[] = {1, 2, 3, 4};
           x[3];
         }));
  ASSERT(16, ({
           int x[] = {1, 2, 3, 4};
           sizeof(x);
         }));
  ASSERT(4, ({
           char x[] = "foo";
           sizeof(x);
         }));

  ASSERT(4, ({
           typedef char T[];
           T x = "foo";
           T y = "x";
           sizeof(x);
         }));
  ASSERT(2, ({
           typedef char T[];
           T x = "foo";
           T y = "x";
           sizeof(y);
         }));
  ASSERT(2, ({
           typedef char T[];
           T x = "x";
           T y = "foo";
           sizeof(x);
         }));
  ASSERT(4, ({
           typedef char T[];
           T x = "x";
           T y = "foo";
           sizeof(y);
         }));

  ASSERT(1, ({
           struct {
             int a;
             int b;
             int c;
           } x = {1, 2, 3};
           x.a;
         }));
  ASSERT(2, ({
           struct {
             int a;
             int b;
             int c;
           } x = {1, 2, 3};
           x.b;
         }));
  ASSERT(3, ({
           struct {
             int a;
             int b;
             int c;
           } x = {1, 2, 3};
           x.c;
         }));
  ASSERT(1, ({
           struct {
             int a;
             int b;
             int c;
           } x = {1};
           x.a;
         }));
  ASSERT(0, ({
           struct {
             int a;
             int b;
             int c;
           } x = {1};
           x.b;
         }));
  ASSERT(0, ({
           struct {
             int a;
             int b;
             int c;
           } x = {1};
           x.c;
         }));

  ASSERT(1, ({
           struct {
             int a;
             int b;
           } x[2] = {{1, 2}, {3, 4}};
           x[0].a;
         }));
  ASSERT(2, ({
           struct {
             int a;
             int b;
           } x[2] = {{1, 2}, {3, 4}};
           x[0].b;
         }));
  ASSERT(3, ({
           struct {
             int a;
             int b;
           } x[2] = {{1, 2}, {3, 4}};
           x[1].a;
         }));
  ASSERT(4, ({
           struct {
             int a;
             int b;
           } x[2] = {{1, 2}, {3, 4}};
           x[1].b;
         }));

  ASSERT(0, ({
           struct {
             int a;
             int b;
           } x[2] = {{1, 2}};
           x[1].b;
         }));

  ASSERT(0, ({
           struct {
             int a;
             int b;
           } x = {};
           x.a;
         }));
  ASSERT(0, ({
           struct {
             int a;
             int b;
           } x = {};
           x.b;
         }));

  ASSERT(5, ({
           typedef struct {
             int a, b, c, d, e, f;
           } T;
           T x = {1, 2, 3, 4, 5, 6};
           T y;
           y = x;
           y.e;
         }));
  ASSERT(2, ({
           typedef struct {
             int a, b;
           } T;
           T x = {1, 2};
           T y, z;
           z = y = x;
           z.b;
         }));

  ASSERT(1, ({
           typedef struct {
             int a, b;
           } T;
           T x = {1, 2};
           T y = x;
           y.a;
         }));

  ASSERT(4, ({
           union {
             int a;
             char b[4];
           } x = {0x01020304};
           x.b[0];
         }));
  ASSERT(3, ({
           union {
             int a;
             char b[4];
           } x = {0x01020304};
           x.b[1];
         }));

  ASSERT(0x01020304, ({
           union {
             struct {
               char a, b, c, d;
             } e;
             int f;
           } x = {{4, 3, 2, 1}};
           x.f;
         }));

  ASSERT(3, g3);
  ASSERT(4, g4);
  ASSERT(5, g5);
  ASSERT(6, g6);

  ASSERT(0, g9[0]);
  ASSERT(1, g9[1]);
  ASSERT(2, g9[2]);

  ASSERT(1, g11[0].a);
  ASSERT(2, g11[0].b);
  ASSERT(3, g11[1].a);
  ASSERT(4, g11[1].b);

  ASSERT(1, g12[0].a[0]);
  ASSERT(2, g12[0].a[1]);
  ASSERT(0, g12[1].a[0]);
  ASSERT(0, g12[1].a[1]);

  ASSERT(4, g13[0].b[0]);
  ASSERT(3, g13[0].b[1]);
  ASSERT(8, g13[1].b[0]);
  ASSERT(7, g13[1].b[1]);

  ASSERT(7, sizeof(g17));
  ASSERT(10, sizeof(g18));
  ASSERT(3, sizeof(g19));

  ASSERT(0, memcmp(g17, "foobar", 7));
  ASSERT(0, memcmp(g18, "foobar\0\0\0", 10));
  ASSERT(0, memcmp(g19, "foo", 3));

  ASSERT(0, strcmp(g20, "foobar"));
  ASSERT(0, strcmp(g21, "bar"));
  ASSERT(0, strcmp(g22 + 3, "foobar"));

  ASSERT(0, strcmp(g23[0], "foobar"));
  ASSERT(0, strcmp(g23[1], "bar"));
  ASSERT(0, strcmp(g23[2] + 3, "foobar"));

  ASSERT(3, g24);
  ASSERT(3, *g25);
  ASSERT(2, *g27);
  ASSERT(3, *g28);
  ASSERT(1, *(int *)g29);

  ASSERT(1, g31[0]);
  ASSERT(2, g31[1]);
  ASSERT(3, g31[2]);

  ASSERT(1, g40[0].a[0]);
  ASSERT(2, g40[0].a[1]);
  ASSERT(3, g40[1].a[0]);
  ASSERT(4, g40[1].a[1]);

  ASSERT(1, g41[0].a[0]);
  ASSERT(2, g41[0].a[1]);
  ASSERT(3, g41[1].a[0]);
  ASSERT(4, g41[1].a[1]);

  ASSERT(0, ({
           int x[2][3] = {0, 1, 2, 3, 4, 5};
           x[0][0];
         }));
  ASSERT(3, ({
           int x[2][3] = {0, 1, 2, 3, 4, 5};
           x[1][0];
         }));

  ASSERT(0, ({
           struct {
             int a;
             int b;
           } x[2] = {0, 1, 2, 3};
           x[0].a;
         }));
  ASSERT(2, ({
           struct {
             int a;
             int b;
           } x[2] = {0, 1, 2, 3};
           x[1].a;
         }));

  ASSERT(0, strcmp(g43[0], "foo"));
  ASSERT(0, strcmp(g43[1], "bar"));
  ASSERT(0, strcmp(g44, "foo"));

  ASSERT(3, ({
           int a[] = {
               1,
               2,
               3,
           };
           a[2];
         }));
  ASSERT(1, ({
           struct {
             int a, b, c;
           } x = {
               1,
               2,
               3,
           };
           x.a;
         }));
  ASSERT(1, ({
           union {
             int a;
             char b;
           } x = {
               1,
           };
           x.a;
         }));
  ASSERT(2, ({
           enum {
             x,
             y,
             z,
           };
           z;
         }));

  ASSERT(3, sizeof(g60));
  ASSERT(6, sizeof(g61));

  ASSERT(4, sizeof(g65));
  ASSERT(7, sizeof(g66));
  ASSERT(0, strcmp(g65.b, "oo"));
  ASSERT(0, strcmp(g66.b, "oobar"));

  ASSERT(4, ({
           int x[3] = {1, 2, 3, [0] = 4, 5};
           x[0];
         }));
  ASSERT(5, ({
           int x[3] = {1, 2, 3, [0] = 4, 5};
           x[1];
         }));
  ASSERT(3, ({
           int x[3] = {1, 2, 3, [0] = 4, 5};
           x[2];
         }));

  ASSERT(10, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0][1] = 7, 8, [0] = 9, [0] = 10, 11, [1][0] = 12};
           x[0][0];
         }));
  ASSERT(11, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0][1] = 7, 8, [0] = 9, [0] = 10, 11, [1][0] = 12};
           x[0][1];
         }));
  ASSERT(8, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0][1] = 7, 8, [0] = 9, [0] = 10, 11, [1][0] = 12};
           x[0][2];
         }));
  ASSERT(12, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0][1] = 7, 8, [0] = 9, [0] = 10, 11, [1][0] = 12};
           x[1][0];
         }));
  ASSERT(5, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0][1] = 7, 8, [0] = 9, [0] = 10, 11, [1][0] = 12};
           x[1][1];
         }));
  ASSERT(6, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0][1] = 7, 8, [0] = 9, [0] = 10, 11, [1][0] = 12};
           x[1][2];
         }));

  ASSERT(7, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0] = {7, 8}, 9, 10};
           x[0][0];
         }));
  ASSERT(8, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0] = {7, 8}, 9, 10};
           x[0][1];
         }));
  ASSERT(3, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0] = {7, 8}, 9, 10};
           x[0][2];
         }));
  ASSERT(9, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0] = {7, 8}, 9, 10};
           x[1][0];
         }));
  ASSERT(10, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0] = {7, 8}, 9, 10};
           x[1][1];
         }));
  ASSERT(6, ({
           int x[2][3] = {1, 2, 3, 4, 5, 6, [0] = {7, 8}, 9, 10};
           x[1][2];
         }));

  ASSERT(7, ((int[10]){[3] = 7})[3]);
  ASSERT(0, ((int[10]){[3] = 7})[4]);

  ASSERT(10, ({
           char x[] = {[10 - 3] = 1, 2, 3};
           sizeof(x);
         }));
  ASSERT(20, ({
           char x[][2] = {[8][1] = 1, 2};
           sizeof(x);
         }));

  ASSERT(3, sizeof(g60));
  ASSERT(6, sizeof(g61));

  ASSERT(4, sizeof(g65));
  ASSERT(7, sizeof(g66));
  ASSERT(0, strcmp(g65.b, "oo"));
  ASSERT(0, strcmp(g66.b, "oobar"));

  ASSERT(7, ((int[10]){[3] 7})[3]);
  ASSERT(0, ((int[10]){[3] 7})[4]);

  ASSERT(4, ({
           struct {
             int a, b;
           } x = {1, 2, .b = 3, .a = 4};
           x.a;
         }));
  ASSERT(3, ({
           struct {
             int a, b;
           } x = {1, 2, .b = 3, .a = 4};
           x.b;
         }));

  ASSERT(1, ({
           struct {
             struct {
               int a, b;
             } c;
           } x = {.c = 1, 2};
           x.c.a;
         }));
  ASSERT(2, ({
           struct {
             struct {
               int a, b;
             } c;
           } x = {.c = 1, 2};
           x.c.b;
         }));

  ASSERT(0, ({
           struct {
             struct {
               int a, b;
             } c;
           } x = {.c.b = 1};
           x.c.a;
         }));
  ASSERT(1, ({
           struct {
             struct {
               int a, b;
             } c;
           } x = {.c.b = 1};
           x.c.b;
         }));

  ASSERT(1, ({
           struct {
             int a[2];
           } x = {.a = 1, 2};
           x.a[0];
         }));
  ASSERT(2, ({
           struct {
             int a[2];
           } x = {.a = 1, 2};
           x.a[1];
         }));

  ASSERT(0, ({
           struct {
             int a[2];
           } x = {.a[1] = 1};
           x.a[0];
         }));
  ASSERT(1, ({
           struct {
             int a[2];
           } x = {.a[1] = 1};
           x.a[1];
         }));

  ASSERT(3, ({
           struct {
             int a, b;
           } x[] = {
               [1].b = 1,
               2,
               [0] = 3,
               4,
           };
           x[0].a;
         }));
  ASSERT(4, ({
           struct {
             int a, b;
           } x[] = {
               [1].b = 1,
               2,
               [0] = 3,
               4,
           };
           x[0].b;
         }));
  ASSERT(0, ({
           struct {
             int a, b;
           } x[] = {
               [1].b = 1,
               2,
               [0] = 3,
               4,
           };
           x[1].a;
         }));
  ASSERT(1, ({
           struct {
             int a, b;
           } x[] = {
               [1].b = 1,
               2,
               [0] = 3,
               4,
           };
           x[1].b;
         }));
  ASSERT(2, ({
           struct {
             int a, b;
           } x[] = {
               [1].b = 1,
               2,
               [0] = 3,
               4,
           };
           x[2].a;
         }));
  ASSERT(0, ({
           struct {
             int a, b;
           } x[] = {
               [1].b = 1,
               2,
               [0] = 3,
               4,
           };
           x[2].b;
         }));

  ASSERT(1, ({
           typedef struct {
             int a, b;
           } T;
           T x = {1, 2};
           T y[] = {x};
           y[0].a;
         }));
  ASSERT(2, ({
           typedef struct {
             int a, b;
           } T;
           T x = {1, 2};
           T y[] = {x};
           y[0].b;
         }));
  ASSERT(0, ({
           typedef struct {
             int a, b;
           } T;
           T x = {1, 2};
           T y[] = {x, [0].b = 3};
           y[0].a;
         }));
  ASSERT(3, ({
           typedef struct {
             int a, b;
           } T;
           T x = {1, 2};
           T y[] = {x, [0].b = 3};
           y[0].b;
         }));

  ASSERT(5, ((struct { int a, b, c; }){.c = 5}).c);
  ASSERT(0, ((struct { int a, b, c; }){.c = 5}).a);

  ASSERT(0x00ff, ({
           union {
             unsigned short a;
             char b[2];
           } x = {.b[0] = 0xff};
           x.a;
         }));
  ASSERT(0xff00, ({
           union {
             unsigned short a;
             char b[2];
           } x = {.b[1] = 0xff};
           x.a;
         }));

  ASSERT(0x00120000, g50.a);
  ASSERT(0, g51[0].a);
  ASSERT(0, g51[1].a);

  ASSERT(1, ({
           struct {
             struct {
               int a;
               struct {
                 int b;
               };
             };
             int c;
           } x = {1, 2, 3, .b = 4, 5};
           x.a;
         }));
  ASSERT(4, ({
           struct {
             struct {
               int a;
               struct {
                 int b;
               };
             };
             int c;
           } x = {1, 2, 3, .b = 4, 5};
           x.b;
         }));
  ASSERT(5, ({
           struct {
             struct {
               int a;
               struct {
                 int b;
               };
             };
             int c;
           } x = {1, 2, 3, .b = 4, 5};
           x.c;
         }));

  ASSERT(16, ({
           char x[] = {[2 ... 10] = 'a', [7] = 'b', [15 ... 15] = 'c', [3 ... 5] = 'd'};
           sizeof(x);
         }));
  ASSERT(0, ({
           char x[] = {[2 ... 10] = 'a', [7] = 'b', [15 ... 15] = 'c', [3 ... 5] = 'd'};
           memcmp(x, "\0\0adddabaaa\0\0\0\0c", 16);
         }));

  return 0;
}
