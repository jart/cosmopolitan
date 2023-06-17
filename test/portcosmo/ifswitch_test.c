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

#define CASEMACRO(X) case X:

static void basicswitch(int value, const char **res) {
  *res = NULL;
  switch (value) {
    case 1: {
      // might create a variable in this scope
      int a = 21;
      *res = "1";
      break;
    }

#ifdef TWO
      CASEMACRO(TWO)
      *res = "TWO";
      // fall-through
#endif

    case THREE: {
      int c = 22;
      *res = "THREE";
      break;
    }

    case -THREE:
      *res = "-THREE";
      break;

    case ~TWO:
      int d = 111;
      *res = "~TWO";
      break;

    case 19 ... 27:
      *res = "19-27";
      break;

    case 0:
      *res = "0";
      // fall-through

    default:
      int z = 12;
      *res = "default";
      break;
  }
  if (*res == NULL) {
    *res = "wut";
  }
}

TEST(portcosmo, ifswitch) {
  const char *r = NULL;

  basicswitch(1, &r);
  ASSERT_STREQ(r, "1");

  basicswitch(TWO, &r);
  ASSERT_STREQ(r, "THREE");  // because fallthrough

  basicswitch(THREE, &r);
  ASSERT_STREQ(r, "THREE");

  basicswitch(-THREE, &r);
  ASSERT_STREQ(r, "-THREE");

  basicswitch(~TWO, &r);
  ASSERT_STREQ(r, "~TWO");

  basicswitch(20, &r);
  ASSERT_STREQ(r, "19-27");

  basicswitch(0, &r);
  ASSERT_STREQ(r, "default"); // because fallthrough

  basicswitch(29, &r);
  ASSERT_STREQ(r, "default");

  basicswitch(999, &r);
  ASSERT_STREQ(r, "default");
}

/* testing switch without default */
static void nodefault(int value, const char **res) {
  *res = NULL;
  switch (value) {
    case 1: {
      *res = "1";
      break;
    }

#ifdef TWO
      CASEMACRO(TWO)
      *res = "TWO";
      break;
#endif

    case THREE: {
      *res = "THREE";
      break;
    }

    case 0:
      *res = "0";
  }
  if (*res == NULL) {
    *res = "wut";
  }
}

TEST(portcosmo, ifswitch_nodefault) {
  const char *r = NULL;

  nodefault(1, &r);
  ASSERT_STREQ(r, "1");

  nodefault(TWO, &r);
  ASSERT_STREQ(r, "TWO");

  nodefault(THREE, &r);
  ASSERT_STREQ(r, "THREE");

  nodefault(0, &r);
  ASSERT_STREQ(r, "0");

  nodefault(29, &r);
  ASSERT_STREQ(r, "wut");

  nodefault(999, &r);
  ASSERT_STREQ(r, "wut");
}
