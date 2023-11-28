#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_STACK_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_STACK_H_
#include "libc/log/check.h"
#include "libc/stdckdint.h"
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/plinko.h"
COSMOPOLITAN_C_START_

#define SetFrame(r, x)  \
  do {                  \
    if (r & NEED_POP) { \
      Repush(x);        \
    } else {            \
      r |= NEED_POP;    \
      Push(x);          \
    }                   \
  } while (0)

forceinline dword GetCurrentFrame(void) {
  DCHECK_GT(sp, 0);
  return g_stack[(sp - 1) & 0xffffffff];
}

forceinline void Push(int x) {
  unsigned short s = sp;
  g_stack[s] = MAKE(x, ~cx);
  if (!ckd_add(&s, s, 1)) {
    sp = s;
  } else {
    StackOverflow();
  }
}

forceinline dword Pop(void) {
  DCHECK_GT(sp, 0);
  return g_stack[--sp & 0xffffffff];
}

forceinline void Repush(int x) {
  int i;
  DCHECK_GT(sp, 0);
  i = (sp - 1) & MASK(STACK);
  g_stack[i & 0xffffffff] = MAKE(x, HI(g_stack[i & 0xffffffff]));
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_STACK_H_ */
