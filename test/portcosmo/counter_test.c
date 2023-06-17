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

/* TODO(ahgamut):
 * enumerator values have to be known at compile time,
 * currently it is not possible to rewrite the AST to
 * handle enums with non-constant values.
 *
    enum circumstances { extenuating = TWO };
 *
 */

static void arbswitch(int value, const char **res) {
  *res = NULL;
  switch (value) {
    case 1: {
      *res = "1";
      break;
    }

    case TWO:
      *res = "TWO";
      break;

    case THREE: {
      *res = "THREE";
      break;
    }

      /*
       * TODO(ahgamut): check if arbitrary expressions in switch-cases
       * are a common thing before updating portcosmo.patch, right now
       * the below case will cause a compilation error:
       *
          case (TWO * THREE):
            *res = "TWO * THREE";
            break;
      */

    case 0:
      *res = "0";
  }
  if (*res == NULL) {
    *res = "wut";
  }
}

TEST(portcosmo, arbitrary_switch) {
  const char *r = NULL;

  arbswitch(1, &r);
  ASSERT_STREQ(r, "1");

  arbswitch(TWO, &r);
  ASSERT_STREQ(r, "TWO");

  arbswitch(THREE, &r);
  ASSERT_STREQ(r, "THREE");

  arbswitch(20, &r);
  ASSERT_STREQ(r, "wut");

  arbswitch(0, &r);
  ASSERT_STREQ(r, "0");

  arbswitch(TWO * THREE, &r);
  /* ASSERT_STREQ(r, "TWO * THREE"); */
  ASSERT_STREQ(r, "wut");
}

struct toy {
  int id;
  int value;
};

TEST(portcosmo, initstruct_WRONG) {
  /* this is an example of portcosmo initializing structs
   * incorrectly. it can only happen if one of the values
   * in the struct initializer before the modified value
   * JUST SO happens to be equal to the temporary value we
   * have #defined somewhere. */
  static struct toy w1 = {.id = -205, .value = TWO};
  /* here -205 JUST SO happens to be equal to __tmpcosmo_TWO,
   * so the struct w1 ends up being initialized with wrong values */

  /* this is what WRONGLY happens,
   * because in portcosmo we are unable
   * to determine the exact (row,col)
   * location of struct fields */
  ASSERT_EQ(TWO, w1.id);      // WRONG!
  ASSERT_EQ(-205, w1.value);  // WRONG!

  /* this is what SHOULD happen */
  ASSERT_NE(-205, w1.id);    // should be ASSERT_EQ
  ASSERT_NE(TWO, w1.value);  // should be ASSERT_EQ
}
