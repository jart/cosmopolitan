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

forceinline unsigned bsfz0(unsigned x) {
  if (x) {
    return bsf(x);
  } else {
    return 0;
  }
}

forceinline unsigned pntz(unsigned p[2]) {
  unsigned r;
  assert(p[0] != 0);
  r = bsfz0(p[0] - 1);
  if (r != 0 ||
      (r = 8 * sizeof(unsigned) + bsfz0(p[1])) != 8 * sizeof(unsigned)) {
    return r;
  }
  return 0;
}

static void cycle(size_t width, unsigned char *ar[], size_t n) {
  unsigned i, l;
  unsigned char tmp[256];
  if (n < 2) return;
  ar[n] = tmp;
  while (width) {
    l = sizeof(tmp) < width ? sizeof(tmp) : width;
    memcpy(ar[n], ar[0], l);
    for (i = 0; i < n; i++) {
      memcpy(ar[i], ar[i + 1], l);
      ar[i] += l;
    }
    width -= l;
  }
}

forceinline void shl(unsigned p[2], size_t n) {
  assert(n > 0);
  if (n >= CHAR_BIT * sizeof(unsigned)) {
    n -= CHAR_BIT * sizeof(unsigned);
    p[1] = p[0];
    p[0] = 0;
  }
  p[1] <<= n;
  p[1] |= p[0] >> (sizeof(unsigned) * CHAR_BIT - n);
  p[0] <<= n;
}

forceinline void shr(unsigned p[2], size_t n) {
  assert(n > 0);
  if (n >= CHAR_BIT * sizeof(unsigned)) {
    n -= CHAR_BIT * sizeof(unsigned);
    p[0] = p[1];
    p[1] = 0;
  }
  p[0] >>= n;
  p[0] |= p[1] << (sizeof(unsigned) * CHAR_BIT - n);
  p[1] >>= n;
}

static void sift(unsigned char *head, cmpfun cmp, void *arg, int pshift,
                 unsigned char *ar[hasatleast 14 * sizeof(unsigned) + 1],
                 unsigned lp[hasatleast 12 * sizeof(unsigned)], size_t width) {
  unsigned i;
  unsigned char *rt, *lf;
  i = 1;
  ar[0] = head;
  while (pshift > 1) {
    rt = head - width;
    lf = head - width - lp[pshift - 2];
    if ((*cmp)(ar[0], lf, arg) >= 0 && (*cmp)(ar[0], rt, arg) >= 0) {
      break;
    }
    if ((*cmp)(lf, rt, arg) >= 0) {
      ar[i++] = lf;
      head = lf;
      pshift -= 1;
    } else {
      ar[i++] = rt;
      head = rt;
      pshift -= 2;
    }
  }
  cycle(width, ar, i);
}

static void trinkle(unsigned char *head, cmpfun cmp, void *arg, unsigned pp[2],
                    unsigned char *ar[hasatleast 14 * sizeof(unsigned) + 1],
                    unsigned lp[hasatleast 12 * sizeof(unsigned)], size_t width,
                    int pshift, int trusty) {
  unsigned p[2];
  unsigned i, trail;
  unsigned char *stepson, *rt, *lf;
  i = 1;
  p[0] = pp[0];
  p[1] = pp[1];
  ar[0] = head;
  while (p[0] != 1 || p[1] != 0) {
    stepson = head - lp[pshift];
    if ((*cmp)(stepson, ar[0], arg) <= 0) {
      break;
    }
    if (!trusty && pshift > 1) {
      rt = head - width;
      lf = head - width - lp[pshift - 2];
      if ((*cmp)(rt, stepson, arg) >= 0 || (*cmp)(lf, stepson, arg) >= 0) {
        break;
      }
    }
    ar[i++] = stepson;
    head = stepson;
    trail = pntz(p);
    shr(p, trail);
    pshift += trail;
    trusty = 0;
  }
  if (!trusty) {
    cycle(width, ar, i);
    sift(head, cmp, arg, pshift, ar, lp, width);
  }
}

/**
 * Smoothsort is an adaptive linearithmic sorting algorithm that's
 * nearly linear on mostly-sorted data, and consumes constant memory.
 */
static noinline void smoothsort(
    void *base, size_t count, size_t width, cmpfun cmp, void *arg,
    unsigned lp[hasatleast 12 * sizeof(unsigned)],
    unsigned char *ar[hasatleast 14 * sizeof(unsigned) + 1]) {
  unsigned i, size = width * count;
  unsigned char *head, *high;
  unsigned p[2] = {1, 0};
  unsigned pshift = 1;
  unsigned trail;
  if (!size) return;
  head = (unsigned char *)base;
  high = head + size - width;
  /* Precompute Leonardo numbers, scaled by element width */
  for (lp[0] = lp[1] = width, i = 2;
       (lp[i] = lp[i - 2] + lp[i - 1] + width) < size; i++) {
  }
  while (head < high) {
    if ((p[0] & 3) == 3) {
      sift(head, cmp, arg, pshift, ar, lp, width);
      shr(p, 2);
      pshift += 2;
    } else {
      if (lp[pshift - 1] >= high - head) {
        trinkle(head, cmp, arg, p, ar, lp, width, pshift, 0);
      } else {
        sift(head, cmp, arg, pshift, ar, lp, width);
      }
      if (pshift == 1) {
        shl(p, 1);
        pshift = 0;
      } else {
        shl(p, pshift - 1);
        pshift = 1;
      }
    }
    p[0] |= 1;
    head += width;
  }
  trinkle(head, cmp, arg, p, ar, lp, width, pshift, 0);
  while (pshift != 1 || p[0] != 1 || p[1] != 0) {
    if (pshift <= 1) {
      trail = pntz(p);
      shr(p, trail);
      pshift += trail;
    } else {
      shl(p, 2);
      pshift -= 2;
      p[0] ^= 7;
      shr(p, 1);
      trinkle(head - lp[pshift] - width, cmp, arg, p, ar, lp, width, pshift + 1,
              1);
      shl(p, 1);
      p[0] |= 1;
      trinkle(head - width, cmp, arg, p, ar, lp, width, pshift, 1);
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
  unsigned lp[12 * sizeof(unsigned)];
  unsigned char *ar[14 * sizeof(unsigned) + 1];
  smoothsort(base, count, width, (cmpfun)cmp, arg, lp, ar);
}

/**
 * Sorts array.
 *
 * @param base points to an array to sort in-place
 * @param count is the item count
 * @param width is the size of each item
 * @param cmp is a callback returning <0, 0, or >0
 * @see qsort_r()
 */
void qsort(void *base, size_t count, size_t width,
           int cmp(const void *, const void *)) {
  qsort_r(base, count, width, (cmpfun)cmp, NULL);
}
