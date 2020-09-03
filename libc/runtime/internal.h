#ifndef COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/dce.h"
#include "libc/runtime/runtime.h"

#define RUNSTATE_INITIALIZED (1 << 0)
#define RUNSTATE_BROKEN      (1 << 1)
#define RUNSTATE_TERMINATE   (1 << 2)

#define STACK_CEIL 0x700000000000ul
#define STACK_SIZE FRAMESIZE

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern bool _mmap_asan_mode;
hidden extern char **g_freebsdhint;
hidden extern unsigned g_runstate;
hidden extern void *g_stacktop;

void _init(void) hidden;
void _piro(int) hidden;
void *__cxa_finalize(void *) hidden;
void _executive(int, char **, char **, long (*)[2]) hidden noreturn;
void __stack_chk_fail(void) noreturn relegated;
void __stack_chk_fail_local(void) noreturn relegated hidden;
int getdosargv(const char16_t *, char *, size_t, char **, size_t) hidden;

forceinline void AssertNeverCalledWhileTerminating(void) {
  if (!NoDebug() && (g_runstate & RUNSTATE_TERMINATE)) {
    abort();
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* ANSI */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_ */
