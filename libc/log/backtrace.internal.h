#ifndef COSMOPOLITAN_LIBC_LOG_BACKTRACE_H_
#define COSMOPOLITAN_LIBC_LOG_BACKTRACE_H_
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/symbols.internal.h"
COSMOPOLITAN_C_START_

forceinline pureconst bool IsValidStackFramePointer(struct StackFrame *x) {
  /* assumes __mmi_lock() is held */
  return IsLegalPointer(x) && !((uintptr_t)x & 15) &&
         (IsStaticStackFrame((uintptr_t)x >> 16) ||
          IsOldStackFrame((uintptr_t)x >> 16) ||
          /* lua coroutines need this */
          IsMemtracked((uintptr_t)x >> 16, (uintptr_t)x >> 16));
}

void ShowBacktrace(int, const struct StackFrame *);
int PrintBacktraceUsingSymbols(int, const struct StackFrame *,
                               struct SymbolTable *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_BACKTRACE_H_ */
