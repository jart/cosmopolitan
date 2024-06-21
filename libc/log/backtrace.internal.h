#ifndef COSMOPOLITAN_LIBC_LOG_BACKTRACE_H_
#define COSMOPOLITAN_LIBC_LOG_BACKTRACE_H_
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/symbols.internal.h"
COSMOPOLITAN_C_START_

void ShowBacktrace(int, const struct StackFrame *);
int PrintBacktraceUsingSymbols(int, const struct StackFrame *,
                               struct SymbolTable *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_BACKTRACE_H_ */
