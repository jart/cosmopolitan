#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_INDEX_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_INDEX_H_
#include "error.h"
#include "plinko.h"
#include "stack.h"

static inline int Head(int x) {
  if (x <= 0)
    return LO(Get(x));
  Push(x);
  Raise(kCar);
}

static inline int Tail(int x) {
  if (x <= 0)
    return HI(Get(x));
  Push(x);
  Raise(kCdr);
}

static inline int Cadr(int x) {
  return Head(Tail(x));
}

static inline int Caddr(int x) {
  return Head(Tail(Tail(x)));
}

static inline int Caar(int X) {
  return Head(Head(X));
}

static inline int Cdar(int X) {
  return Tail(Head(X));
}

static inline int Cddr(int X) {
  return Tail(Tail(X));
}

static inline int Caaar(int X) {
  return Head(Head(Head(X)));
}

static inline int Caadr(int X) {
  return Head(Head(Tail(X)));
}

static inline int Cadar(int X) {
  return Head(Tail(Head(X)));
}

static inline int Cdaar(int X) {
  return Tail(Head(Head(X)));
}

static inline int Cdadr(int X) {
  return Tail(Head(Tail(X)));
}

static inline int Cddar(int X) {
  return Tail(Tail(Head(X)));
}

static inline int Cdddr(int X) {
  return Tail(Tail(Tail(X)));
}

static inline int Caaaar(int X) {
  return Head(Head(Head(Head(X))));
}

static inline int Caaadr(int X) {
  return Head(Head(Head(Tail(X))));
}

static inline int Caadar(int X) {
  return Head(Head(Tail(Head(X))));
}

static inline int Caaddr(int X) {
  return Head(Head(Tail(Tail(X))));
}

static inline int Cadaar(int X) {
  return Head(Tail(Head(Head(X))));
}

static inline int Cadadr(int X) {
  return Head(Tail(Head(Tail(X))));
}

static inline int Caddar(int X) {
  return Head(Tail(Tail(Head(X))));
}

static inline int Cadddr(int X) {
  return Head(Tail(Tail(Tail(X))));
}

static inline int Cdaaar(int X) {
  return Tail(Head(Head(Head(X))));
}

static inline int Cdaadr(int X) {
  return Tail(Head(Head(Tail(X))));
}

static inline int Cdadar(int X) {
  return Tail(Head(Tail(Head(X))));
}

static inline int Cdaddr(int X) {
  return Tail(Head(Tail(Tail(X))));
}

static inline int Cddaar(int X) {
  return Tail(Tail(Head(Head(X))));
}

static inline int Cddadr(int X) {
  return Tail(Tail(Head(Tail(X))));
}

static inline int Cdddar(int X) {
  return Tail(Tail(Tail(Head(X))));
}

static inline int Cddddr(int X) {
  return Tail(Tail(Tail(Tail(X))));
}

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_INDEX_H_ */
