#ifndef COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/dce.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/runtime/ezmap.internal.h"
#include "libc/runtime/runtime.h"

#define STACK_CEIL 0x700000000000ul
#define STACK_SIZE FRAMESIZE

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern hidden bool _mmap_asan_mode;
extern hidden char **g_freebsdhint;
extern hidden void *g_stacktop;

void _init(void) hidden;
void _piro(int) hidden;
void *__cxa_finalize(void *) hidden;
void _executive(int, char **, char **, long (*)[2]) hidden wontreturn;
void __stack_chk_fail(void) wontreturn relegated;
void __stack_chk_fail_local(void) wontreturn relegated hidden;
void _jmpstack(void *, void *, ...) hidden wontreturn;
long _setstack(void *, void *, ...) hidden;
int GetDosArgv(const char16_t *, char *, size_t, char **, size_t) hidden;
Elf64_Ehdr *MapElfRead(const char *, struct MappedFile *) hidden;
int GetDosEnviron(const char16_t *, char *, size_t, char **, size_t) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* ANSI */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_ */
