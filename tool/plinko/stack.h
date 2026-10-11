#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_STACK_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_STACK_H_
#include <stdckdint.h>
#include "error.h"
#include "plinko.h"

#define SetFrame(r, x)  \
  do {                  \
    if (r & NEED_POP) { \
      Repush(x);        \
    } else {            \
      r |= NEED_POP;    \
      Push(x);          \
    }                   \
  } while (0)

static inline dword GetCurrentFrame(void) {
  assert(sp > 0);
  return g_stack[(sp - 1) & 0xffffffff];
}

static inline void Push(int x) {
  wchar_t s = sp;
  g_stack[s] = MAKE(x, ~cx);
  if (!ckd_add(&s, s, 1)) {
    sp = s;
  } else {
    StackOverflow();
  }
}

static inline dword Pop(void) {
  assert(sp > 0);
  return g_stack[--sp & 0xffffffff];
}

static inline void Repush(int x) {
  int i;
  assert(sp > 0);
  i = (sp - 1) & MASK(STACK);
  g_stack[i & 0xffffffff] = MAKE(x, HI(g_stack[i & 0xffffffff]));
}

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_STACK_H_ */
