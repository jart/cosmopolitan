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
#include "libc/thread/tls.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define A TLS_ALIGNMENT

long z = 2;
pthread_t t;
_Thread_local long x;
_Thread_local long y[1] = {40};
_Alignas(A) _Thread_local long a;

dontubsan void *Worker(void *arg) {
  ASSERT_EQ(A, _Alignof(a));
  ASSERT_EQ(0, (uintptr_t)&a & (_Alignof(a) - 1));
  ASSERT_EQ(42, x + y[0] + z);
  ASSERT_EQ(0, (intptr_t)&a & (A - 1));
  if (IsAsan()) {
    ASSERT_EQ(kAsanProtected, __asan_check(y + 1, sizeof(long)).kind);
  }
  return 0;
}

TEST(tls, test) {
  ASSERT_EQ(A, _Alignof(a));
  ASSERT_EQ(0, (uintptr_t)&a & (_Alignof(a) - 1));
  ASSERT_EQ(0, sizeof(struct CosmoTib) % A);
  ASSERT_EQ(0, (intptr_t)__get_tls() & (A - 1));
  EXPECT_EQ(2, z);
  EXPECT_EQ(40, y[0]);
  EXPECT_EQ(42, x + y[0] + z);
  y[0] = 666;
  ASSERT_EQ(0, (intptr_t)&a & (A - 1));
  ASSERT_EQ(0, pthread_create(&t, 0, Worker, 0));
  ASSERT_EQ(0, pthread_join(t, 0));
  if (IsAsan()) {
    // TODO(jart): Why isn't it poisoned?
    // ASSERT_EQ(kAsanProtected, __asan_check(y + 1, sizeof(long)).kind);
  }
}
