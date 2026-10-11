#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_GC_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_GC_H_
#include "types.h"

struct Gc {
  int A, B, C;
  unsigned n;
  unsigned noop;
  unsigned *P;
  dword M[];
};

int MarkSweep(int, int);
struct Gc *NewGc(int);
int Census(struct Gc *);
void Sweep(struct Gc *);
void Marker(const dword[], int, int);
int Relocater(const dword[], const unsigned[], int, int);

static inline int Relocate(const struct Gc *G, int x) {
  if (x >= G->C)
    return x;
  return Relocater(G->M, G->P, G->A, x);
}

static inline void Mark(struct Gc *G, int x) {
  if (x >= G->A)
    return;
  Marker(G->M, G->A, x);
}

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_GC_H_ */
