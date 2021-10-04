/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│ Copyright (C) 2011 by Valentin Ochs                                          │
│                                                                              │
│ Permission is hereby granted, free of charge, to any person obtaining a copy │
│ of this software and associated documentation files (the "Software"), to     │
│ deal in the Software without restriction, including without limitation the   │
│ rights to use, copy, modify, merge, publish, distribute, sublicense, and/or  │
│ sell copies of the Software, and to permit persons to whom the Software is   │
│ furnished to do so, subject to the following conditions:                     │
│                                                                              │
│ The above copyright notice and this permission notice shall be included in   │
│ all copies or substantial portions of the Software.                          │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   │
│ IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     │
│ FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  │
│ AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       │
│ LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      │
│ FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS │
│ IN THE SOFTWARE.                                                             │
└─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/alg.h"
#include "libc/assert.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
Smoothsort (MIT License)\\n\
Copyright 2011 Valentin Ochs\\n\
Discovered by Edsger Dijkstra\"");
asm(".include \"libc/disclaimer.inc\"");

typedef int (*cmpfun)(const void *, const void *, void *);

struct SmoothSort {
  size_t lp[12 * sizeof(size_t)];
  unsigned char *ar[14 * sizeof(size_t) + 1];
  unsigned char tmp[256];
};

static inline int ntz(unsigned long x) {
  return __builtin_ctzl(x);
}

static inline int pntz(size_t p[2]) {
  int r = ntz(p[0] - 1);
  if (r != 0 || (r = CHAR_BIT * sizeof(size_t) + ntz(p[1])) !=
                    CHAR_BIT * sizeof(size_t)) {
    return r;
  }
  return 0;
}

/* smoothsort_shl() and smoothsort_shr() need n > 0 */
static inline void smoothsort_shl(size_t p[2], int n) {
  if (n >= CHAR_BIT * sizeof(size_t)) {
    n -= CHAR_BIT * sizeof(size_t);
    p[1] = p[0];
    p[0] = 0;
  }
  p[1] <<= n;
  p[1] |= p[0] >> (sizeof(size_t) * CHAR_BIT - n);
  p[0] <<= n;
}

static inline void smoothsort_shr(size_t p[2], int n) {
  if (n >= CHAR_BIT * sizeof(size_t)) {
    n -= CHAR_BIT * sizeof(size_t);
    p[0] = p[1];
    p[1] = 0;
  }
  p[0] >>= n;
  p[0] |= p[1] << (sizeof(size_t) * CHAR_BIT - n);
  p[1] >>= n;
}

static void smoothsort_cycle(struct SmoothSort *s, size_t width, int n) {
  size_t l;
  int i;
  if (n < 2) {
    return;
  }
  s->ar[n] = s->tmp;
  while (width) {
    l = sizeof(s->tmp) < width ? sizeof(s->tmp) : width;
    memcpy(s->ar[n], s->ar[0], l);
    for (i = 0; i < n; i++) {
      memcpy(s->ar[i], s->ar[i + 1], l);
      s->ar[i] += l;
    }
    width -= l;
  }
}

static void smoothsort_sift(struct SmoothSort *s, unsigned char *head,
                            size_t width, cmpfun cmp, void *arg, int pshift) {
  unsigned char *rt, *lf;
  int i = 1;
  s->ar[0] = head;
  while (pshift > 1) {
    rt = head - width;
    lf = head - width - s->lp[pshift - 2];
    if (cmp(s->ar[0], lf, arg) >= 0 && cmp(s->ar[0], rt, arg) >= 0) {
      break;
    }
    if (cmp(lf, rt, arg) >= 0) {
      s->ar[i++] = lf;
      head = lf;
      pshift -= 1;
    } else {
      s->ar[i++] = rt;
      head = rt;
      pshift -= 2;
    }
  }
  smoothsort_cycle(s, width, i);
}

static void smoothsort_trinkle(struct SmoothSort *s, unsigned char *head,
                               size_t width, cmpfun cmp, void *arg,
                               size_t pp[2], int pshift, int trusty) {
  unsigned char *stepson, *rt, *lf;
  size_t p[2];
  int i = 1;
  int trail;
  p[0] = pp[0];
  p[1] = pp[1];
  s->ar[0] = head;
  while (p[0] != 1 || p[1] != 0) {
    stepson = head - s->lp[pshift];
    if (cmp(stepson, s->ar[0], arg) <= 0) {
      break;
    }
    if (!trusty && pshift > 1) {
      rt = head - width;
      lf = head - width - s->lp[pshift - 2];
      if (cmp(rt, stepson, arg) >= 0 || cmp(lf, stepson, arg) >= 0) {
        break;
      }
    }
    s->ar[i++] = stepson;
    head = stepson;
    trail = pntz(p);
    smoothsort_shr(p, trail);
    pshift += trail;
    trusty = 0;
  }
  if (!trusty) {
    smoothsort_cycle(s, width, i);
    smoothsort_sift(s, head, width, cmp, arg, pshift);
  }
}

static void smoothsort(struct SmoothSort *s, void *base, size_t nel,
                       size_t width, cmpfun cmp, void *arg) {
  size_t i, size = width * nel;
  unsigned char *head, *high;
  size_t p[2] = {1, 0};
  int pshift = 1;
  int trail;
  if (!size) return;
  head = base;
  high = head + size - width;
  /* Precompute Leonardo numbers, scaled by element width */
  for (s->lp[0] = s->lp[1] = width, i = 2;
       (s->lp[i] = s->lp[i - 2] + s->lp[i - 1] + width) < size; i++) {
  }
  while (head < high) {
    if ((p[0] & 3) == 3) {
      smoothsort_sift(s, head, width, cmp, arg, pshift);
      smoothsort_shr(p, 2);
      pshift += 2;
    } else {
      if (s->lp[pshift - 1] >= high - head) {
        smoothsort_trinkle(s, head, width, cmp, arg, p, pshift, 0);
      } else {
        smoothsort_sift(s, head, width, cmp, arg, pshift);
      }
      if (pshift == 1) {
        smoothsort_shl(p, 1);
        pshift = 0;
      } else {
        smoothsort_shl(p, pshift - 1);
        pshift = 1;
      }
    }
    p[0] |= 1;
    head += width;
  }
  smoothsort_trinkle(s, head, width, cmp, arg, p, pshift, 0);
  while (pshift != 1 || p[0] != 1 || p[1] != 0) {
    if (pshift <= 1) {
      trail = pntz(p);
      smoothsort_shr(p, trail);
      pshift += trail;
    } else {
      smoothsort_shl(p, 2);
      pshift -= 2;
      p[0] ^= 7;
      smoothsort_shr(p, 1);
      smoothsort_trinkle(s, head - s->lp[pshift] - width, width, cmp, arg, p,
                         pshift + 1, 1);
      smoothsort_shl(p, 1);
      p[0] |= 1;
      smoothsort_trinkle(s, head - width, width, cmp, arg, p, pshift, 1);
    }
    head -= width;
  }
}

/**
 * Sorts array.
 *
 * @param base points to an array to sort in-place
 * @param count is the item count
 * @param width is the size of each item
 * @param cmp is a callback returning <0, 0, or >0
 * @param arg will optionally be passed as the third argument to cmp
 * @see qsort()
 */
void qsort_r(void *base, size_t count, size_t width, cmpfun cmp, void *arg) {
  struct SmoothSort s;
  smoothsort(&s, base, count, width, cmp, arg);
}

/**
 * Sorts array.
 *
 * @param base points to an array to sort in-place
 * @param count is the item count
 * @param width is the size of each item
 * @param cmp is a callback returning <0, 0, or >0
 * @see longsort(), djbsort()
 */
void qsort(void *base, size_t count, size_t width,
           int cmp(const void *, const void *)) {
  struct SmoothSort s;
  smoothsort(&s, base, count, width, (cmpfun)cmp, 0);
}
