#ifndef COSMOPOLITAN_LIBC_LOG_GETCALLERNAME_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_GETCALLERNAME_INTERNAL_H_
#include "libc/nexgen32e/stackframe.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *GetCallerName(const struct StackFrame *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_GETCALLERNAME_INTERNAL_H_ */
