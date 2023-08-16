/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"

__static_yoink("zipos");
__static_yoink("libc/testlib/hyperion.txt");
__static_yoink("inflate");
__static_yoink("inflateInit2");
__static_yoink("inflateEnd");

int Worker(void *arg, int tid) {
  int i, fd;
  char *data;
  for (i = 0; i < 20; ++i) {
    ASSERT_NE(-1, (fd = open("/zip/libc/testlib/hyperion.txt", O_RDONLY)));
    data = malloc(kHyperionSize);
    ASSERT_EQ(kHyperionSize, read(fd, data, kHyperionSize));
    ASSERT_EQ(0, memcmp(data, kHyperion, kHyperionSize));
    ASSERT_SYS(0, 0, close(fd));
    free(data);
  }
  return 0;
}

TEST(zipos, test) {
  int i, n = 16;
  struct spawn *t = _gc(malloc(sizeof(struct spawn) * n));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, _spawn(Worker, 0, t + i));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, _join(t + i));
  __print_maps();
}

TEST(zipos, normpath) {
  {
    char s[] = "";
    __zipos_normpath(s);
    ASSERT_STREQ("", s);
  }
  {
    char s[] = "usr/";
    __zipos_normpath(s);
    ASSERT_STREQ("usr", s);
  }
  {
    char s[] = "usr/./";
    __zipos_normpath(s);
    ASSERT_STREQ("usr", s);
  }
}

#if 0
TEST(zipos_O_DIRECTORY, blocksOpeningOfNormalFiles) {
  ASSERT_SYS(ENOTDIR, -1,
             open("/zip/libc/testlib/hyperion.txt", O_RDONLY | O_DIRECTORY));
}
#endif

TEST(zipos, readPastEof) {
  char buf[512];
  ASSERT_SYS(0, 3, open("/zip/libc/testlib/hyperion.txt", O_RDONLY));
  EXPECT_SYS(EINVAL, -1, pread(3, buf, 512, UINT64_MAX));
  EXPECT_SYS(0, 0, pread(3, buf, 512, INT64_MAX));
  EXPECT_SYS(EINVAL, -1, lseek(3, UINT64_MAX, SEEK_SET));
  EXPECT_SYS(0, INT64_MAX, lseek(3, INT64_MAX, SEEK_SET));
  EXPECT_SYS(0, 0, read(3, buf, 512));
  EXPECT_SYS(0, 0, close(3));
}
