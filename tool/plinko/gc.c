#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "gc.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "cons.h"
#include "error.h"
#include "histo.h"
#include "plinko.h"
#include "print.h"
#include "printf.h"
#include "stack.h"

#define MAX_(X, Y)    ((Y) < (X) ? (X) : (Y))
#define ROUNDUP(X, K) (((X) + (K) - 1) & -(K))

static uint_least64_t popcnt(uint_least64_t x) {
  x = x - ((x >> 1) & 0x5555555555555555);
  x = ((x >> 2) & 0x3333333333333333) + (x & 0x3333333333333333);
  x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
  x = (x + (x >> 32)) & 0xffffffff;
  x = x + (x >> 16);
  x = (x + (x >> 8)) & 0x0000007f;
  return x;
}

static int bsfl(uint_least64_t x) {
  unsigned l, r;
  x &= -x;
  l = x | x >> 32;
  r = !!(x >> 32), r <<= 1;
  r += !!((l & 0xffff0000)), r <<= 1;
  r += !!((l & 0xff00ff00)), r <<= 1;
  r += !!((l & 0xf0f0f0f0)), r <<= 1;
  r += !!((l & 0xcccccccc)), r <<= 1;
  r += !!((l & 0xaaaaaaaa));
  return r;
}

static inline void SetBit(dword M[], unsigned i) {
  M[i / DWBITS] |= (dword)1 << (i % DWBITS);
}

static inline bool HasBit(const dword M[], unsigned i) {
  return (M[i / DWBITS] >> (i % DWBITS)) & 1;
}

static struct Gc *g_gc;
static size_t g_gcsize;

struct Gc *NewGc(int A) {
  int B = cx;
  unsigned n;
  size_t size;
  struct Gc *G;
  assert(B <= A);
  assert(A <= 0);
  if (B < cHeap)
    cHeap = B;
  n = ROUNDUP(A - B, DWBITS) / DWBITS;
  size = sizeof(*G) + n * sizeof(G->M[0]) + (n + 1) * sizeof(*G->P);
  if (size > g_gcsize) {
    free(g_gc);
    if (!(g_gc = malloc((size = MAX_(size, g_gcsize * 2)))))
      OutOfMemory();
    g_gcsize = size;
  }
  G = g_gc;
  memset(G->M, 0, n * sizeof(G->M[0]));
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
    if (HasBit(M, i))
      return;
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
      l += bsfl(~G->M[j]);
      break;
    }
  }
  G->C = G->A - l;
#if HISTO_GARBAGE
  HISTO(g_gc_marks_histogram, t);
  HISTO(g_gc_discards_histogram, i - t);
  HISTO(g_gc_lop_histogram, l);
#endif
  if (gtrace)
    Fprintf(2, L";; gc %'d conses, %'d kept, %'d freed%n", i, t, i - t);
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
  if (G->noop)
    return;
  i = 0;
  b = d = G->A;
  for (; i < G->n; ++i) {
    m = G->M[i];
    if (~m) {
      j = bsfl(~m);
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
      a = b + ~(j = bsfl(m));
      Set(--d, MAKE(Relocate(G, LO(Get(a))), Relocate(G, HI(Get(a)))));
      SetShadow(d, MAKE(LO(GetShadow(a)), Relocate(G, HI(GetShadow(a)))));
    }
  }
  cx = d;
}

int MarkSweep(int A, int x) {
  struct Gc *G;
  if (x >= A)
    return cx = A, x;
  G = NewGc(A);
  Mark(G, x);
  Census(G);
  x = Relocate(G, x);
  Sweep(G);
  return x;
}
