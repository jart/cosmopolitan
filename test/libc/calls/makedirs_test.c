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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(makedirs, empty) {
  ASSERT_SYS(ENOENT, -1, makedirs("", 0755));
}

TEST(makedirs, isfile) {
  ASSERT_SYS(0, 0, xbarf("f", "hi", -1));
  ASSERT_SYS(EEXIST, -1, makedirs("f", 0755));
}

TEST(makedirs, isdir) {
  ASSERT_SYS(0, 0, mkdir("d", 0755));
  ASSERT_SYS(0, 0, makedirs("d", 0755));
}

TEST(makedirs, enotdir) {
  ASSERT_SYS(0, 0, xbarf("f", "hi", -1));
  ASSERT_SYS(ENOTDIR, -1, makedirs("f/d", 0755));
}

TEST(makedirs, basic) {
  ASSERT_SYS(0, 0, makedirs("a/b/c", 0755));
  ASSERT_TRUE(isdirectory("a/b/c"));
  ASSERT_SYS(0, 0, makedirs("a/b/c/d/e/", 0755));
  ASSERT_SYS(0, 0, makedirs("a/b/c/d/e/", 0755));
  ASSERT_TRUE(isdirectory("a/b/c/d/e"));
}

#define DIR \
  "a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z/A/B/C/D/E/F/G/H/I/J"

pthread_barrier_t barrier;

void *Worker(void *arg) {
  pthread_barrier_wait(&barrier);
  ASSERT_EQ(0, makedirs(DIR, 0755));
  return 0;
}

TEST(makedirs, test) {
  int i, n = 8;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, n));
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  for (i = 0; i < n; ++i) EXPECT_EQ(0, pthread_join(t[i], 0));
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}
