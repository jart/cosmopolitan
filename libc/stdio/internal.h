#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/stdio/stdio.h"

#define PUSHBACK 12

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

_Hide extern uint64_t g_rando;

int __fflush_impl(FILE *) _Hide;
int __fflush_register(FILE *) _Hide;
void __fflush_unregister(FILE *) _Hide;
bool __stdio_isok(FILE *) _Hide;
FILE *__stdio_alloc(void) _Hide;
void __stdio_free(FILE *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
