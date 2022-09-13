/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"
#include "libc/x/x.h"

#define GC(x) _defer(Free, x)

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
  const char *dir;
  if (!strcmp(path, "/") || !strcmp(path, ".")) return;
  crawl(_gc(xdirname(path)));
}

int Worker(void *arg, int tid) {
  crawl("a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d");
  return 0;
}

TEST(gc, torture) {
  int i, n = 32;
  struct spawn *t = gc(malloc(sizeof(struct spawn) * n));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, _spawn(Worker, 0, t + i));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, _join(t + i));
}

void crawl2(jmp_buf jb, const char *path) {
  const char *dir;
  if (!strcmp(path, "/") || !strcmp(path, ".")) _gclongjmp(jb, 1);
  crawl2(jb, _gc(xdirname(path)));
}

int Worker2(void *arg, int tid) {
  jmp_buf jb;
  if (!setjmp(jb)) {
    crawl2(jb, "a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d/a/b/c/d");
  }
  return 0;
}

TEST(_gclongjmp, torture) {
  int i, n = 32;
  struct spawn *t = gc(malloc(sizeof(struct spawn) * n));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, _spawn(Worker2, 0, t + i));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, _join(t + i));
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
