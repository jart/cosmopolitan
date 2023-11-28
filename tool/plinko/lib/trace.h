#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_TRACE_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_TRACE_H_
#include "libc/str/str.h"
COSMOPOLITAN_C_START_

#define START_TRACE                    \
  bool t;                              \
  PairFn *pf;                          \
  BindFn *bf;                          \
  EvlisFn *ef;                         \
  RecurseFn *rf;                       \
  TailFn *tails[8];                    \
  EvalFn *ev, *ex;                     \
  memcpy(tails, kTail, sizeof(kTail)); \
  ev = eval;                           \
  bf = bind_;                          \
  ef = evlis;                          \
  ex = expand;                         \
  pf = pairlis;                        \
  rf = recurse;                        \
  EnableTracing();                     \
  t = trace;                           \
  trace = true

#define END_TRACE \
  trace = t;      \
  eval = ev;      \
  bind_ = bf;     \
  evlis = ef;     \
  expand = ex;    \
  pairlis = pf;   \
  recurse = rf;   \
  memcpy(kTail, tails, sizeof(kTail))

void EnableTracing(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_TRACE_H_ */
