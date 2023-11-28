#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_INDEX_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_INDEX_H_
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/stack.h"
COSMOPOLITAN_C_START_

forceinline nosideeffect int Head(int x) {
  if (x <= 0) return LO(Get(x));
  Push(x);
  Raise(kCar);
}

forceinline nosideeffect int Tail(int x) {
  if (x <= 0) return HI(Get(x));
  Push(x);
  Raise(kCdr);
}

forceinline nosideeffect int Cadr(int x) {
  return Head(Tail(x));
}

forceinline nosideeffect int Caddr(int x) {
  return Head(Tail(Tail(x)));
}

static inline nosideeffect int Caar(int X) {
  return Head(Head(X));
}

static inline nosideeffect int Cdar(int X) {
  return Tail(Head(X));
}

static inline nosideeffect int Cddr(int X) {
  return Tail(Tail(X));
}

static inline nosideeffect int Caaar(int X) {
  return Head(Head(Head(X)));
}

static inline nosideeffect int Caadr(int X) {
  return Head(Head(Tail(X)));
}

static inline nosideeffect int Cadar(int X) {
  return Head(Tail(Head(X)));
}

static inline nosideeffect int Cdaar(int X) {
  return Tail(Head(Head(X)));
}

static inline nosideeffect int Cdadr(int X) {
  return Tail(Head(Tail(X)));
}

static inline nosideeffect int Cddar(int X) {
  return Tail(Tail(Head(X)));
}

static inline nosideeffect int Cdddr(int X) {
  return Tail(Tail(Tail(X)));
}

static inline nosideeffect int Caaaar(int X) {
  return Head(Head(Head(Head(X))));
}

static inline nosideeffect int Caaadr(int X) {
  return Head(Head(Head(Tail(X))));
}

static inline nosideeffect int Caadar(int X) {
  return Head(Head(Tail(Head(X))));
}

static inline nosideeffect int Caaddr(int X) {
  return Head(Head(Tail(Tail(X))));
}

static inline nosideeffect int Cadaar(int X) {
  return Head(Tail(Head(Head(X))));
}

static inline nosideeffect int Cadadr(int X) {
  return Head(Tail(Head(Tail(X))));
}

static inline nosideeffect int Caddar(int X) {
  return Head(Tail(Tail(Head(X))));
}

static inline nosideeffect int Cadddr(int X) {
  return Head(Tail(Tail(Tail(X))));
}

static inline nosideeffect int Cdaaar(int X) {
  return Tail(Head(Head(Head(X))));
}

static inline nosideeffect int Cdaadr(int X) {
  return Tail(Head(Head(Tail(X))));
}

static inline nosideeffect int Cdadar(int X) {
  return Tail(Head(Tail(Head(X))));
}

static inline nosideeffect int Cdaddr(int X) {
  return Tail(Head(Tail(Tail(X))));
}

static inline nosideeffect int Cddaar(int X) {
  return Tail(Tail(Head(Head(X))));
}

static inline nosideeffect int Cddadr(int X) {
  return Tail(Tail(Head(Tail(X))));
}

static inline nosideeffect int Cdddar(int X) {
  return Tail(Tail(Tail(Head(X))));
}

static inline nosideeffect int Cddddr(int X) {
  return Tail(Tail(Tail(Tail(X))));
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_INDEX_H_ */
