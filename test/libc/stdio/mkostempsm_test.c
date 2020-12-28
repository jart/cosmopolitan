/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/temp.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

#define MODE()                   \
  ({                             \
    va_list va;                  \
    unsigned Mode;               \
    va_start(va, flags);         \
    Mode = va_arg(va, unsigned); \
    va_end(va);                  \
    Mode;                        \
  })

static int MockOpen1(const char *file, int flags, ...) {
  static bool once;
  ASSERT_FALSE(once);
  once = true;
  EXPECT_STREQ("/tmp/mkostemps.ctre5m", file);
  EXPECT_EQ(O_RDWR | O_CREAT | O_EXCL, flags);
  EXPECT_EQ(0600, MODE());
  return 123;
}

TEST(mkostempsm, test1) {
  uint64_t rando = 1;
  char path[PATH_MAX] = "/tmp/mkostemps.XXXXXX";
  EXPECT_EQ(123L, mkostempsmi(path, 0, 0, &rando, 0600, MockOpen1));
  EXPECT_STREQ("/tmp/mkostemps.ctre5m", path);
}

static int MockOpen2(const char *file, int flags, ...) {
  static int state;
  switch (state) {
    case 0:
      state = 1;
      EXPECT_STREQ("/tmp/mkostemps.ctre5m", file);
      EXPECT_EQ((unsigned)(O_RDWR | O_CREAT | O_EXCL), flags);
      EXPECT_EQ(0600, MODE());
      errno = EEXIST;
      return -1;
    case 1:
      state = 1;
      EXPECT_STREQ("/tmp/mkostemps.jl1h61", file);
      EXPECT_EQ((unsigned)(O_RDWR | O_CREAT | O_EXCL), flags);
      EXPECT_EQ(0600, MODE());
      return 123;
    default:
      abort();
  }
}

TEST(mkostempsm, test2) {
  uint64_t rando = 1;
  char path[PATH_MAX] = "/tmp/mkostemps.XXXXXX";
  EXPECT_EQ(123, mkostempsmi(path, 0, 0, &rando, 0600, MockOpen2));
  EXPECT_STREQ("/tmp/mkostemps.jl1h61", path);
}
