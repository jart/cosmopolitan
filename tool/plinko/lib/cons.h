#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_CONS_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_CONS_H_
#include "libc/stdckdint.h"
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/types.h"
COSMOPOLITAN_C_START_

forceinline void Set(int i, dword t) {
#ifndef NDEBUG
  DCHECK_NE(0, i);
  DCHECK_LT(i, TERM);
  DCHECK_LT(LO(t), TERM);
  DCHECK_LE(HI(t), TERM);
  if (i < 0) {
    DCHECK_LT(i, LO(t), "topology compromised");
    DCHECK_LT(i, HI(t), "topology compromised");
  } else {
    DCHECK_GE(LO(t), 0);
    DCHECK_GE(HI(t), 0);
  }
#endif
  g_mem[i & (BANE | MASK(BANE))] = t;
  ++cSets;
}

forceinline int Alloc(dword t) {
  int c = cx;
  if (!ckd_sub(&c, c, 1)) {
    Set(c, t);
    return cx = c;
  }
  OutOfMemory();
}

forceinline void SetShadow(int i, dword t) {
#ifndef NDEBUG
  DCHECK_GE(i, cx);
  DCHECK_LT(i, TERM);
  DCHECK_GE(LO(t), 0);
  /* if (i < 0) DCHECK_GE(HI(t), i, "topology compromised"); */
#endif
  ((__seg_fs dword *)((uintptr_t)g_mem))[i & (BANE | MASK(BANE))] = t;
}

forceinline int Cons(int x, int y) {
  int c;
  c = Alloc(MAKE(x, y));
  SetShadow(c, DF(DispatchPlan));
  return c;
}

forceinline int Alist(int x, int y, int z) {
  return Cons(Cons(x, y), z);
}

int List(int, int);
int List3(int, int, int);
int List4(int, int, int, int);
int Shadow(int, int);
int GetCommonCons(int, int);
int ShareCons(int, int);
int ShareList(int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_CONS_H_ */
