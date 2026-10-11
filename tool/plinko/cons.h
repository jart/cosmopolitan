#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_CONS_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_CONS_H_
#include "error.h"
#include "plinko.h"
#include "types.h"

static inline void Set(int i, dword t) {
#ifndef NDEBUG
  assert(0 != i);
  assert(i < TERM);
  assert(LO(t) < TERM);
  assert(HI(t) <= TERM);
  if (i < 0) {
    assert(i < LO(t));
    assert(i < HI(t));
  } else {
    assert(LO(t) >= 0);
    assert(HI(t) >= 0);
  }
#endif
  g_mem[i] = t;
  ++cSets;
}

static inline int Alloc(dword t) {
  int c = cx;
  if (c <= cFloor)
    Grow();
  Set(--c, t);
  return cx = c;
}

static inline void SetShadow(int i, dword t) {
#ifndef NDEBUG
  assert(i >= cx);
  assert(i < TERM);
  assert(LO(t) >= 0);
  /* if (i < 0) DCHECK_GE(HI(t), i, "topology compromised"); */
#endif
  GetShadow(i) = t;
}

static inline int Cons(int x, int y) {
  int c;
  c = Alloc(MAKE(x, y));
  SetShadow(c, DF(DispatchPlan));
  return c;
}

static inline int Alist(int x, int y, int z) {
  return Cons(Cons(x, y), z);
}

int List(int, int);
int List3(int, int, int);
int Shadow(int, int);
int GetCommonCons(int, int);
int ShareCons(int, int);
int ShareList(int, int);

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_CONS_H_ */
