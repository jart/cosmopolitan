/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

#define PATH "hog"

FILE *f;
char buf[512];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(fwrite, test) {
  ASSERT_NE(NULL, (f = fopen(PATH, "wb")));
  EXPECT_EQ(-1, fgetc(f));
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(EBADF, errno);
  ASSERT_EQ(EBADF, ferror(f));
  clearerr(f);
  EXPECT_EQ(5, fwrite("hello", 1, 5, f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  EXPECT_EQ(0, fwrite("hello", 1, 5, f));
  EXPECT_EQ(EBADF, ferror(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "a+b")));
  EXPECT_EQ(5, fwrite("hello", 1, 5, f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  EXPECT_EQ(10, fread(buf, 1, 10, f));
  EXPECT_TRUE(!memcmp(buf, "hellohello", 10));
  EXPECT_NE(-1, fclose(f));
}

TEST(fwrite, testSmallBuffer) {
  ASSERT_NE(NULL, (f = fopen(PATH, "wb")));
  setbuffer(f, gc(malloc(1)), 1);
  EXPECT_EQ(-1, fgetc(f));
  clearerr(f);
  EXPECT_EQ(5, fwrite("hello", 1, 5, f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setbuffer(f, gc(malloc(1)), 1);
  EXPECT_EQ(0, fwrite("hello", 1, 5, f));
  EXPECT_EQ(EBADF, ferror(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "a")));
  setbuffer(f, gc(malloc(1)), 1);
  EXPECT_EQ(5, fwrite("hello", 1, 5, f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setbuffer(f, gc(malloc(1)), 1);
  EXPECT_EQ(10, fread(buf, 1, 10, f));
  EXPECT_TRUE(!memcmp(buf, "hellohello", 10));
  EXPECT_NE(-1, fclose(f));
}

TEST(fwrite, testLineBuffer) {
  ASSERT_NE(NULL, (f = fopen(PATH, "wb")));
  setvbuf(f, NULL, _IOLBF, 64);
  EXPECT_EQ(-1, fgetc(f));
  clearerr(f);
  EXPECT_EQ(5, fwrite("heyy\n", 1, 5, f));
  EXPECT_EQ(0, fread(buf, 0, 0, f));
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(0, fread(buf, 1, 10, f));
  EXPECT_EQ(EBADF, ferror(f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setvbuf(f, NULL, _IOLBF, 64);
  EXPECT_EQ(0, fwrite("heyy\n", 1, 5, f));
  EXPECT_EQ(EBADF, ferror(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "a")));
  setvbuf(f, NULL, _IOLBF, 64);
  EXPECT_EQ(5, fwrite("heyy\n", 1, 5, f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setvbuf(f, NULL, _IOLBF, 64);
  EXPECT_EQ(10, fread(buf, 1, 10, f));
  EXPECT_TRUE(!memcmp(buf, "heyy\nheyy\n", 10));
  EXPECT_NE(-1, fclose(f));
}

TEST(fwrite, testNoBuffer) {
  ASSERT_NE(NULL, (f = fopen(PATH, "wb")));
  setvbuf(f, NULL, _IONBF, 64);
  EXPECT_EQ(-1, fgetc(f));
  clearerr(f);
  EXPECT_EQ(5, fwrite("heyy\n", 1, 5, f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setvbuf(f, NULL, _IONBF, 64);
  EXPECT_EQ(0, fwrite("heyy\n", 1, 5, f));
  EXPECT_EQ(EBADF, ferror(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "a")));
  setvbuf(f, NULL, _IONBF, 64);
  EXPECT_EQ(5, fwrite("heyy\n", 1, 5, f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setvbuf(f, NULL, _IONBF, 64);
  EXPECT_EQ(10, fread(buf, 1, 10, f));
  EXPECT_TRUE(!memcmp(buf, "heyy\nheyy\n", 10));
  EXPECT_NE(-1, fclose(f));
}

void MeatyReadWriteTest(void) {
  size_t n;
  char *mem, *buf;
  n = 8 * 1024 * 1024;
  buf = gc(malloc(n));
  mem = rngset(gc(malloc(n)), n, _rand64, -1);
  ASSERT_NE(NULL, (f = fopen(PATH, "wb")));
  setbuffer(f, gc(malloc(4 * 1000 * 1000)), 4 * 1000 * 1000);
  EXPECT_EQ(n, fwrite(mem, 1, n, f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen(PATH, "r")));
  setbuffer(f, gc(malloc(4 * 1000 * 1000)), 4 * 1000 * 1000);
  EXPECT_EQ(n, fread(buf, 1, n, f));
  EXPECT_EQ(0, memcmp(buf, mem, n));
  EXPECT_NE(-1, fclose(f));
}

volatile bool gotsigint;
void OnSigInt(int sig) {
  gotsigint = true;
}

TEST(fwrite, signalStorm) {
  if (IsWindows()) return;
  int pid;
  struct sigaction oldchld, oldint;
  struct sigaction sachld = {.sa_handler = SIG_IGN};
  struct sigaction saint = {.sa_handler = OnSigInt};
  EXPECT_NE(-1, sigaction(SIGINT, &saint, &oldint));
  EXPECT_NE(-1, sigaction(SIGCHLD, &sachld, &oldchld));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    do {
      ASSERT_NE(-1, kill(getppid(), SIGINT));
      usleep(5000);
    } while (!gotsigint);
    _exit(0);
  }
  pause();
  MeatyReadWriteTest();
  EXPECT_NE(-1, kill(pid, SIGINT));
  while (wait(0) == -1 && errno == EINTR) donothing;
  EXPECT_NE(-1, sigaction(SIGCHLD, &oldchld, NULL));
  EXPECT_NE(-1, sigaction(SIGINT, &oldint, NULL));
}
