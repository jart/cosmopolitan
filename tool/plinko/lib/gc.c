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
#include "tool/plinko/lib/gc.h"
#include "libc/assert.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/popcnt.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/countbranch.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/histo.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/print.h"
#include "tool/plinko/lib/printf.h"
#include "tool/plinko/lib/stack.h"

forceinline void SetBit(dword M[], unsigned i) {
  M[i / DWBITS] |= (dword)1 << (i % DWBITS);
}

forceinline nosideeffect bool HasBit(const dword M[], unsigned i) {
  return (M[i / DWBITS] >> (i % DWBITS)) & 1;
}

struct Gc *NewGc(int A) {
  int B = cx;
  unsigned n;
  struct Gc *G;
  DCHECK_LE(B, A);
  DCHECK_LE(A, 0);
  if (B < cHeap) cHeap = B;
  n = ROUNDUP(A - B, DWBITS) / DWBITS;
  G = Addr(BANE);
  bzero(G->M, n * sizeof(G->M[0]));
  G->n = n;
  G->A = A;
  G->B = B;
  G->P = (unsigned *)(G->M + n);
  *G->P++ = 0;
  return G;
}

void Marker(const dword M[], int A, int x) {
  int i;
  dword t;
  do {
    i = ~(x - A);
    if (HasBit(M, i)) return;
    SetBit((void *)M, i);
    if (HI(GetShadow(x)) < A) {
      Marker(M, A, HI(GetShadow(x)));
    }
    t = Get(x);
    if (LO(t) < A) {
      Marker(M, A, LO(t));
    }
  } while ((x = HI(t)) < A);
}

int Census(struct Gc *G) {
  int n, t, l;
  unsigned i, j;
  i = G->A - G->B;
  n = i / DWBITS;
  for (j = t = 0; j < n; ++j) {
    G->P[j] = t += popcnt(G->M[j]);
  }
  if (i % DWBITS) {
    t += popcnt(G->M[j]);
  }
  G->noop = t == i;
  for (l = j = 0; j < G->n; ++j) {
    if (!~G->M[j]) {
      l += DWBITS;
    } else {
      l += _bsfl(~G->M[j]);
      break;
    }
  }
  G->C = G->A - l;
#if HISTO_GARBAGE
  HISTO(g_gc_marks_histogram, t);
  HISTO(g_gc_discards_histogram, i - t);
  HISTO(g_gc_lop_histogram, l);
#endif
  return t;
}

int Relocater(const dword M[], const unsigned P[], int A, int x) {
  long n;
  unsigned i, r;
  i = ~(x - A);
  n = i / DWBITS;
  r = i % DWBITS;
  return A + ~(P[n - 1] + popcnt(M[n] & (((dword)1 << r) - 1)));
}

void Sweep(struct Gc *G) {
  dword m;
  int a, b, d, i, j;
  if (G->noop) return;
  i = 0;
  b = d = G->A;
  for (; i < G->n; ++i) {
    m = G->M[i];
    if (~m) {
      j = _bsfl(~m);
      m >>= j;
      m <<= j;
      d -= j;
      break;
    } else {
      b -= DWBITS;
      d -= DWBITS;
    }
  }
  for (; i < G->n; b -= DWBITS, m = G->M[++i]) {
    for (; m; m &= ~((dword)1 << j)) {
      a = b + ~(j = _bsfl(m));
      Set(--d, MAKE(Relocate(G, LO(Get(a))), Relocate(G, HI(Get(a)))));
      SetShadow(d, MAKE(LO(GetShadow(a)), Relocate(G, HI(GetShadow(a)))));
    }
  }
  cx = d;
}

int MarkSweep(int A, int x) {
  struct Gc *G;
  if (x >= A) return cx = A, x;
  G = NewGc(A);
  Mark(G, x);
  Census(G);
  x = Relocate(G, x);
  Sweep(G);
  return x;
}
