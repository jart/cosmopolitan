/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

__static_yoink("zipos");
__static_yoink("libc/testlib/hyperion.txt");
__static_yoink("_Cz_inflate");
__static_yoink("_Cz_inflateInit2");
__static_yoink("_Cz_inflateEnd");

void *Worker(void *arg) {
  int i, fd;
  char *data;
  for (i = 0; i < 20; ++i) {
    ASSERT_NE(-1, (fd = open("/zip//./libc/testlib//hyperion.txt", O_RDONLY)));
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
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 0, pthread_create(t + i, 0, Worker, 0));
  }
  for (i = 0; i < n; ++i) {
    EXPECT_SYS(0, 0, pthread_join(t[i], 0));
  }
  __print_maps();
}

TEST(zipos, erofs) {
  ASSERT_SYS(EROFS, -1, creat("/zip/foo.txt", 0644));
}

TEST(zipos, enoent) {
  ASSERT_SYS(ENOENT, -1, open("/zip/foo.txt", O_RDONLY));
}

TEST(zipos, readPastEof) {
  char buf[512];
  ASSERT_SYS(0, 3, open("/zip/libc/testlib/hyperion.txt", O_RDONLY));
  EXPECT_SYS(EINVAL, -1, lseek(3, -1, SEEK_CUR));
  EXPECT_SYS(EINVAL, -1, lseek(3, -1, SEEK_SET));
  EXPECT_SYS(EINVAL, -1, pread(3, buf, 512, UINT64_MAX));
  EXPECT_SYS(0, 0, pread(3, buf, 512, INT64_MAX));
  EXPECT_SYS(0, INT64_MAX, lseek(3, INT64_MAX, SEEK_SET));
  EXPECT_SYS(EOVERFLOW, -1, lseek(3, 2, SEEK_CUR));
  EXPECT_SYS(EBADF, -1, write(3, buf, 512));
  EXPECT_SYS(EBADF, -1, pwrite(3, buf, 512, 0));
  EXPECT_SYS(0, 0, read(3, buf, 512));
  EXPECT_SYS(0, 0, close(3));
}

TEST(zipos_O_DIRECTORY, blocksOpeningOfNormalFiles) {
  ASSERT_SYS(ENOTDIR, -1,
             open("/zip/libc/testlib/hyperion.txt", O_RDONLY | O_DIRECTORY));
}

TEST(zipos, trailingComponents_willEnodirFile) {
  struct stat st;
  ASSERT_SYS(ENOTDIR, -1, open("/zip/libc/testlib/hyperion.txt/", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("/zip/libc/testlib/hyperion.txt/.", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("/zip/libc/testlib/hyperion.txt/./", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("/zip/libc/testlib/hyperion.txt/a/b", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, stat("/zip/libc/testlib/hyperion.txt/", &st));
}

TEST(zipos, lseek) {
  char b1[512], b2[512];
  ASSERT_SYS(0, 3, open("/zip/libc/testlib/hyperion.txt", O_RDONLY));
  EXPECT_SYS(0, 512, pread(3, b2, 512, 512 - 200));
  EXPECT_SYS(0, 512, read(3, b1, 512));
  EXPECT_SYS(0, 512, lseek(3, 0, SEEK_CUR));
  EXPECT_SYS(0, 512 - 200, lseek(3, -200, SEEK_CUR));
  EXPECT_SYS(0, 512, read(3, b1, 512));
  EXPECT_EQ(0, memcmp(b1, b2, 512));
  EXPECT_SYS(0, 0, close(3));
}

TEST(zipos, closeAfterVfork) {
  ASSERT_SYS(0, 3, open("/zip/libc/testlib/hyperion.txt", O_RDONLY));
  SPAWN(vfork);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("/dev/null", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(EBADF, -1, close(3));
  EXITS(0);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(EBADF, -1, close(3));
}

struct State {
  int id;
  int fd;
  pthread_t thread;
};

#define READS()                         \
  for (int i = 0; i < 4; ++i) {         \
    char buf[8];                        \
    ASSERT_SYS(0, 8, read(fd, buf, 8)); \
  }
#define SEEKS()                  \
  for (int i = 0; i < 4; ++i) {  \
    rc = lseek(fd, 8, SEEK_CUR); \
    ASSERT_NE(rc, -1);           \
  }

static void *pthread_main(void *ptr) {
  struct State *s = ptr;
  struct State children[2];
  int fd, rc;

  fd = s->fd;
  if (s->id < 8) {
    for (int i = 0; i < 2; ++i) {
      rc = dup(fd);
      ASSERT_NE(-1, rc);
      children[i].fd = rc;
      children[i].id = 2 * s->id + i;
      ASSERT_SYS(0, 0,
                 pthread_create(&children[i].thread, NULL, pthread_main,
                                children + i));
    }
  }
  if (s->id & 1) {
    SEEKS();
    READS();
  } else {
    READS();
    SEEKS();
  }
  ASSERT_SYS(0, 0, close(fd));
  if (s->id < 8) {
    for (int i = 0; i < 2; ++i) {
      ASSERT_SYS(0, 0, pthread_join(children[i].thread, NULL));
    }
  }
  return NULL;
}

TEST(zipos, ultraPosixAtomicSeekRead) {
  struct State s = {1, 4};
  ASSERT_SYS(0, 3, open("/zip/libc/testlib/hyperion.txt", O_RDONLY));
  ASSERT_SYS(0, 4, dup(3));
  pthread_main((void *)&s);
  EXPECT_EQ(960, lseek(3, 0, SEEK_CUR));
  ASSERT_SYS(0, 0, close(3));
}
