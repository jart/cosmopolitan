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
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"
#ifdef __x86_64__
// TODO(jart): get gclongjmp() working properly on aarch64

#define GC(x) defer(Free, x)

char *x;
char *y;
char *z;
jmp_buf jb;

void Free(char *p) {
  strcpy(p, "FREE");
}

void C(void) {
  x = GC(strdup("abcd"));
  if (0) PrintGarbage();
  gclongjmp(jb, 1);
  abort();
}

void B(void C(void)) {
  y = GC(strdup("HIHI"));
  C();
  abort();
}

void A(void C(void), void B(void(void))) {
  z = GC(strdup("yoyo"));
  B(C);
  abort();
}

void (*Ap)(void(void), void(void(void))) = A;
void (*Bp)(void(void)) = B;
void (*Cp)(void) = C;

TEST(gclongjmp, test) {
  if (0) PrintGarbage();
  if (!setjmp(jb)) {
    Ap(Cp, Bp);
    abort();
  }
  if (0) PrintGarbage();
  EXPECT_STREQ("FREE", x);
  EXPECT_STREQ("FREE", y);
  EXPECT_STREQ("FREE", z);
  free(z);
  free(y);
  free(x);
}

void crawl(const char *path) {
  if (!strcmp(path, "/") || !strcmp(path, ".")) return;
  crawl(gc(xdirname(path)));
}

void *Worker(void *arg) {
  crawl("a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d");
  return 0;
}

TEST(gc, torture) {
  int i, n = 32;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, pthread_create(t + i, 0, Worker, 0));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, pthread_join(t[i], 0));
}

#if defined(__GNUC__) && __GNUC__ >= 12
#pragma GCC diagnostic ignored "-Winfinite-recursion"
#endif

void crawl2(jmp_buf jb, const char *path) {
  if (!strcmp(path, "/") || !strcmp(path, ".")) gclongjmp(jb, 1);
  crawl2(jb, gc(xdirname(path)));
}

void *Worker2(void *arg) {
  jmp_buf jb;
  if (!setjmp(jb)) {
    crawl2(jb, "a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d");
  }
  return 0;
}

TEST(gclongjmp, torture) {
  int i, n = 32;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 0, pthread_create(t + i, 0, Worker2, 0));
  }
  for (i = 0; i < n; ++i) {
    EXPECT_SYS(0, 0, pthread_join(t[i], 0));
  }
}

dontinline void F1(void) {
  /* 3x slower than F2() but sooo worth it */
  gc(malloc(16));
}

dontinline void F2(void) {
  void *volatile p;
  p = malloc(16);
  free(p);
}

void (*F1p)(void) = F1;
void (*F2p)(void) = F2;

BENCH(gc, bench) {
  EZBENCH2("gc(malloc(16))", donothing, F1p());
  EZBENCH2("free(malloc(16))", donothing, F2p());
}

#endif /* __x86_64__ */
