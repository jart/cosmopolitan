/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/isystem/stdio.h"
#include "libc/isystem/string.h"
#include "libc/testlib/testlib.h"
#include "test/portcosmo/constants.h"

struct toy {
  int id;
  int value;
};

struct toyroom {
  int id;
  int value;
  struct toy x1;
  struct toy x2;
  struct toy x3;
  struct toy x4;
};

static int v1 = TWO;
static int arr1[] = {TWO, THREE, ~THREE, ~TWO};
struct toy t0 = {.id = 30, .value = THREE};
static struct toy t1 = {.id = 31, .value = TWO};
static struct toy ta[] = {{.id = 1, .value = ~TWO},
                          {.id = ~THREE, .value = TWO},
                          {.value = TWO, .id = -THREE},
                          {.id = THREE, .value = 1},
                          {.id = 7, .value = THREE}};
static struct toyroom r1 = {
    .id = 2,
    .value = -TWO,
    .x1 = {.id = 1, .value = TWO},
    .x2 = {.id = ~TWO, .value = 1},
    .x3 = {.value = TWO, .id = -THREE},
    .x4 = {.id = THREE, .value = ~THREE},
};

TEST(portcosmo, initstruct_global) {
  ASSERT_EQ(v1, TWO);

  ASSERT_EQ(arr1[0], TWO);
  ASSERT_EQ(arr1[1], THREE);
  ASSERT_EQ(arr1[2], ~THREE);
  ASSERT_EQ(arr1[3], ~TWO);

  ASSERT_EQ(t1.id, 31);
  ASSERT_EQ(t1.value, TWO);

  ASSERT_EQ(ta[0].id, 1);
  ASSERT_EQ(ta[0].value, ~TWO);
  ASSERT_EQ(ta[1].id, ~THREE);
  ASSERT_EQ(ta[1].value, TWO);
  ASSERT_EQ(ta[2].id, -THREE);
  ASSERT_EQ(ta[2].value, TWO);
  ASSERT_EQ(ta[3].id, THREE);
  ASSERT_EQ(ta[3].value, 1);
  ASSERT_EQ(ta[4].id, 7);
  ASSERT_EQ(ta[4].value, THREE);

  ASSERT_EQ(r1.id, 2);
  ASSERT_EQ(r1.value, -TWO);
  ASSERT_EQ(r1.x1.id, 1);
  ASSERT_EQ(r1.x1.value, TWO);
  ASSERT_EQ(r1.x2.id, ~TWO);
  ASSERT_EQ(r1.x2.value, 1);
  ASSERT_EQ(r1.x3.id, -THREE);
  ASSERT_EQ(r1.x3.value, TWO);
  ASSERT_EQ(r1.x4.id, THREE);
  ASSERT_EQ(r1.x4.value, ~THREE);
}

void init_func() {
  static struct toy box[] = {
      {.id = 1, .value = 1},       {.id = -THREE, .value = THREE},
      {.id = THREE, .value = 1},   {.id = -TWO, .value = ~TWO},
      {.id = TWO, .value = THREE}, {.id = ~THREE, .value = TWO},
  };
  static struct toyroom r2 = {
      .id = 2,
      .value = THREE,
      .x1 = {.id = 1, .value = TWO},
      .x2 = {.id = ~TWO, .value = 1},
      .x3 = {.id = TWO, .value = ~THREE},
      .x4 = {.id = THREE, .value = -THREE},
  };

  static struct toy t = {.id = 22, .value = TWO};
  static int v = TWO;
  static int vals[] = {1, TWO, -THREE, ~TWO, 1};
  static int count = 0;

  ASSERT_EQ(box[0].id, 1);
  ASSERT_EQ(box[0].value, 1);
  ASSERT_EQ(box[1].id, -THREE);
  ASSERT_EQ(box[1].value, THREE);
  ASSERT_EQ(box[2].id, THREE);
  ASSERT_EQ(box[2].value, 1);
  ASSERT_EQ(box[3].id, -TWO);
  ASSERT_EQ(box[3].value, ~TWO);
  ASSERT_EQ(box[4].id, TWO);
  ASSERT_EQ(box[4].value, THREE);
  ASSERT_EQ(box[5].id, ~THREE);
  ASSERT_EQ(box[5].value, TWO);

  ASSERT_EQ(r2.id, 2);
  ASSERT_EQ(r2.value, THREE);
  ASSERT_EQ(r2.x1.id, 1);
  ASSERT_EQ(r2.x1.value, TWO);
  ASSERT_EQ(r2.x2.id, ~TWO);
  ASSERT_EQ(r2.x2.value, 1);
  ASSERT_EQ(r2.x3.id, TWO);
  ASSERT_EQ(r2.x3.value, ~THREE);
  ASSERT_EQ(r2.x4.id, THREE);
  ASSERT_EQ(r2.x4.value, -THREE);

  ASSERT_EQ(t.id, 22);
  ASSERT_EQ(t.value, TWO + count);

  ASSERT_EQ(vals[0], 1 + 5 * count);
  ASSERT_EQ(vals[1], TWO + 5 * count);
  ASSERT_EQ(vals[2], -THREE + 5 * count);
  ASSERT_EQ(vals[3], ~TWO + 5 * count);
  ASSERT_EQ(vals[4], 1 + 5 * count);

  /* when we call this function a second time,
   * we can confirm that the values were not
   * wrongly initialized twice */
  for (int i = 0; i < 5; ++i) {
    vals[i] += 5;
  }
  t.value += 1;
  count += 1;
}

TEST(portcosmo, initstruct_local) {
  init_func();
  init_func();
}
