#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "plinko.h"

#define MIN_(X, Y) ((Y) > (X) ? (X) : (Y))

// room for atoms (positive indices) above the conses (negative ones)
#define ATOMS (TERM + 1)

static long g_mem_conses;
static long g_shadow_conses;

/**
 * Resizes array so it can hold `m` conses, where `*n` is its capacity.
 *
 * The array holds conses at negative indices and atoms at positive
 * indices, so growing it means sliding everything up by the amount it
 * grew. Since the heap doubles each time, the total amount of copying
 * never exceeds twice the size of the final heap.
 */
static bool Resize(dword **p, long *n, long m) {
  dword *b;
  if (!*p) {
    if (!(b = calloc(m + ATOMS, sizeof(dword))))
      return false;
  } else {
    if (!(b = realloc(*p - *n, (m + ATOMS) * sizeof(dword))))
      return false;
    memmove(b + (m - *n), b, (*n + ATOMS) * sizeof(dword));
  }
  *p = b + m;
  *n = m;
  return true;
}

/**
 * Doubles the number of conses the heap can hold.
 */
void Grow(void) {
  long n, m;
  bool ok1, ok2;
  n = -(long)cFloor;
  m = n ? MIN_(n * 2, -(long)INT_MIN) : HEAP;
  if (m == n)
    OutOfMemory();
  ok1 = Resize(&g_heap_mem, &g_mem_conses, m);
  ok2 = Resize(&g_heap_shadow, &g_shadow_conses, m);
  RestoreHeap();
  if (!ok1 || !ok2)
    OutOfMemory();
  cFloor = -m;
}

/**
 * Reloads the heap base registers from their canonical copies.
 *
 * This needs to happen after longjmp(), which restores whatever values
 * the callee-saved registers had when setjmp() was called.
 */
void RestoreHeap(void) {
  g_mem = g_heap_mem;
  g_shadow = g_heap_shadow;
}
