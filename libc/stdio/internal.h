#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/stdio/stdio.h"

#define PUSHBACK 12

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern uint64_t g_rando;

int __fflush_impl(FILE *);
int __fflush_register(FILE *);
void __fflush_unregister(FILE *);
bool __stdio_isok(FILE *);
FILE *__stdio_alloc(void);
void __stdio_free(FILE *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
