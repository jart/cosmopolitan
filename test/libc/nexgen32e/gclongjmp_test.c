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
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
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
