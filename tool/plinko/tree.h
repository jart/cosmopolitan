#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_TREE_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_TREE_H_
#include "cons.h"
#include "plinko.h"

int PutTree(int, int, int);
int GetTree(int, int);
int GetTreeCount(int, int, int *);
int Nod(int, int, int, int);

static inline int Key(int E) {
  return E < 0 ? Car(E) : E;
}

static inline int Val(int E) {
  return E < 0 ? Cdr(E) : E;
}

static inline int Ent(int N) {
  return Car(Car(N));
}

static inline int Chl(int N) {
  return Cdr(Car(N));
}

static inline int Lit(int N) {
  return Car(Chl(N));
}

static inline int Rit(int N) {
  return Cdr(Chl(N));
}

static inline int Red(int N) {
  return Cdr(N);
}

static inline int Bkn(int N) {
  return Red(N) ? Cons(Car(N), 0) : N;
}

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_TREE_H_ */
