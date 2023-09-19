#ifndef COSMOPOLITAN_LIBC_CALLS_CONSOLE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_CONSOLE_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/nt/struct/inputrecord.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int CountConsoleInputBytes(int64_t);
int ConvertConsoleInputToAnsi(const struct NtInputRecord *, char[hasatleast 32],
                              uint16_t *, struct Fd *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_CONSOLE_INTERNAL_H_ */
