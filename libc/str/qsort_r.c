/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 1992, 1993                                                     │
│      The Regents of the University of California.  All rights reserved.      │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions           │
│ are met:                                                                     │
│  1. Redistributions of source code must retain the above copyright           │
│     notice, this list of conditions and the following disclaimer.            │
│  2. Redistributions in binary form must reproduce the above copyright        │
│     notice, this list of conditions and the following disclaimer in the      │
│     documentation and/or other materials provided with the distribution.     │
│  3. Neither the name of the University nor the names of its contributors     │
│     may be used to endorse or promote products derived from this software    │
│     without specific prior written permission.                               │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND      │
│ ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE        │
│ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   │
│ ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE     │
│ FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   │
│ DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS      │
│ OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)        │
│ HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   │
│ LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    │
│ OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       │
│ SUCH DAMAGE.                                                                 │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
NetBSD qsort (BSD-3)\\n\
Copyright 1992 The Regents of the University of California\"");
asm(".include \"libc/disclaimer.inc\"");

#define SWAPINIT(a, es)                                                      \
  swaptype = ((char *)a - (char *)0) % sizeof(long) || es % sizeof(long) ? 2 \
             : es == sizeof(long)                                        ? 0 \
                                                                         : 1;

#define swapcode(TYPE, parmi, parmj, n) \
  size_t i = (n) / sizeof(TYPE);        \
  TYPE *pi = (TYPE *)(void *)(parmi);   \
  TYPE *pj = (TYPE *)(void *)(parmj);   \
  do {                                  \
    TYPE t = *pi;                       \
    *pi++ = *pj;                        \
    *pj++ = t;                          \
  } while (--i > 0)

#define swap(a, b)                               \
  if (swaptype == 0) {                           \
    long t = *(long *)(void *)(a);               \
    *(long *)(void *)(a) = *(long *)(void *)(b); \
    *(long *)(void *)(b) = t;                    \
  } else                                         \
    swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n) \
  if ((n) > 0) swapfunc((a), (b), (size_t)(n), swaptype)

static inline void swapfunc(char *a, char *b, size_t n, int swaptype) {
  if (swaptype <= 1) {
    swapcode(long, a, b, n);
  } else {
    swapcode(char, a, b, n);
  }
}

static inline char *med3(char *a, char *b, char *c,
                         int (*cmp)(const void *, const void *, void *),
                         void *z) {
  if (cmp(a, b, z) < 0) {
    return cmp(b, c, z) < 0 ? b : (cmp(a, c, z) < 0 ? c : a);
  } else {
    return cmp(b, c, z) > 0 ? b : (cmp(a, c, z) < 0 ? a : c);
  }
}

/**
 * Sorts array.
 *
 * This implementation uses the Quicksort routine from Bentley &
 * McIlroy's "Engineering a Sort Function", 1992, Bell Labs. This
 * implementation is also used on systems like NetBSD and MacOS.
 *
 * @param a is base which points to an array to sort in-place
 * @param n is item count
 * @param es is width of each item
 * @param cmp is a callback returning <0, 0, or >0
 * @param arg will optionally be passed as the third argument to cmp
 * @see smoothsort_r()
 */
void qsort_r(void *a, size_t n, size_t es,
             int (*cmp)(const void *, const void *, void *), void *arg) {
  size_t d, r, s;
  int swaptype, cmp_result;
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;

loop:
  SWAPINIT(a, es);
  if (n < 7) {
    for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
      for (pl = pm; pl > (char *)a && cmp(pl - es, pl, arg) > 0; pl -= es)
        swap(pl, pl - es);
    return;
  }

  pm = (char *)a + (n / 2) * es;
  if (n > 7) {
    pl = (char *)a;
    pn = (char *)a + (n - 1) * es;
    if (n > 40) {
      d = (n / 8) * es;
      pl = med3(pl, pl + d, pl + 2 * d, cmp, arg);
      pm = med3(pm - d, pm, pm + d, cmp, arg);
      pn = med3(pn - 2 * d, pn - d, pn, cmp, arg);
    }
    pm = med3(pl, pm, pn, cmp, arg);
  }
  swap(a, pm);
  pa = pb = (char *)a + es;
  pc = pd = (char *)a + (n - 1) * es;

  for (;;) {
    while (pb <= pc && (cmp_result = cmp(pb, a, arg)) <= 0) {
      if (cmp_result == 0) {
        swap(pa, pb);
        pa += es;
      }
      pb += es;
    }
    while (pb <= pc && (cmp_result = cmp(pc, a, arg)) >= 0) {
      if (cmp_result == 0) {
        swap(pc, pd);
        pd -= es;
      }
      pc -= es;
    }
    if (pb > pc) break;
    swap(pb, pc);
    pb += es;
    pc -= es;
  }

  pn = (char *)a + n * es;
  r = MIN(pa - (char *)a, pb - pa);
  vecswap(a, pb - r, r);
  r = MIN((size_t)(pd - pc), pn - pd - es);
  vecswap(pb, pn - r, r);

  /*
   * To save stack space we sort the smaller side of the partition first
   * using recursion and eliminate tail recursion for the larger side.
   */
  r = pb - pa;
  s = pd - pc;
  if (r < s) {
    /* Recurse for 1st side, iterate for 2nd side. */
    if (s > es) {
      if (r > es) qsort_r(a, r / es, es, cmp, arg);
      a = pn - s;
      n = s / es;
      goto loop;
    }
  } else {
    /* Recurse for 2nd side, iterate for 1st side. */
    if (r > es) {
      if (s > es) qsort_r(pn - s, s / es, es, cmp, arg);
      n = r / es;
      goto loop;
    }
  }
}
