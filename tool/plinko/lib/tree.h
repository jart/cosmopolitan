#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_TREE_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_TREE_H_
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/plinko.h"
COSMOPOLITAN_C_START_

int PutTree(int, int, int);
int GetTree(int, int);
int GetTreeCount(int, int, int *);
int Nod(int, int, int, int);

forceinline pureconst int Key(int E) {
  return E < 0 ? Car(E) : E;
}

forceinline pureconst int Val(int E) {
  return E < 0 ? Cdr(E) : E;
}

forceinline pureconst int Ent(int N) {
  return Car(Car(N));
}

forceinline pureconst int Chl(int N) {
  return Cdr(Car(N));
}

forceinline pureconst int Lit(int N) {
  return Car(Chl(N));
}

forceinline pureconst int Rit(int N) {
  return Cdr(Chl(N));
}

forceinline pureconst int Red(int N) {
  return Cdr(N);
}

forceinline int Bkn(int N) {
  return Red(N) ? Cons(Car(N), 0) : N;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_TREE_H_ */
