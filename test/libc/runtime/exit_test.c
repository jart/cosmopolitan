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
#include "libc/runtime/runtime.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

int i, *p;

void SetUp(void) {
  p = _mapshared(FRAMESIZE);
}

void TearDown(void) {
  munmap(p, FRAMESIZE);
}

void AtExit3(void) {
  p[i++] = 3;
}

void AtExit2(void) {
  p[i++] = 2;
  exit(2);
}

void AtExit1(void) {
  p[i++] = 1;
  atexit(AtExit2);
  exit(1);
}

// consistent with glibc, musl, freebsd, openbsd & netbsd
// please note posix says recursion is undefined behavior
// however, fifo ordering of atexit handlers is specified
TEST(exit, test) {
  SPAWN(fork);
  atexit(AtExit3);
  atexit(AtExit3);
  atexit(AtExit1);
  exit(0);
  EXITS(2);
  ASSERT_EQ(1, p[0]);
  ASSERT_EQ(2, p[1]);
  ASSERT_EQ(3, p[2]);
  ASSERT_EQ(3, p[3]);
}
