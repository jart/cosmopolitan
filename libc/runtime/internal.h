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

extern int __pid;
extern uint32_t __ntconsolemode[3];
extern const char v_ntsubsystem[] __attribute__((__weak__));
extern const uintptr_t __fini_array_end[] __attribute__((__weak__));
extern const uintptr_t __fini_array_start[] __attribute__((__weak__));

extern unsigned char _tdata_start[];
extern unsigned char _tdata_end[];
extern unsigned char _tdata_size[];
extern unsigned char _tbss_start[];
extern unsigned char _tbss_end[];
extern unsigned char _tbss_size[];
extern unsigned char _tbss_offset[];
extern unsigned char _tls_size[];
extern unsigned char _tls_content[];

void _init(void) _Hide;
void __morph_tls(void);
void __enable_tls(void);
void __enable_threads(void) _Hide;
void *__cxa_finalize(void *) _Hide;
void cosmo(int, char **, char **, long (*)[2]) _Hide wontreturn;
void __stack_chk_fail(void) wontreturn relegated;
void __stack_chk_fail_local(void) wontreturn relegated _Hide;
void _jmpstack(void *, void *, ...) _Hide wontreturn;
long _setstack(void *, void *, ...) _Hide;
int GetDosArgv(const char16_t *, char *, size_t, char **, size_t);
Elf64_Ehdr *MapElfRead(const char *, struct MappedFile *) _Hide;
int GetDosEnviron(const char16_t *, char *, size_t, char **, size_t);
bool __intercept_flag(int *, char *[], const char *);
int sys_mprotect_nt(void *, size_t, int) _Hide;
int __inflate(void *, size_t, const void *, size_t);
noasan void *_Mmap(void *addr, size_t size, int prot, int flags, int fd,
                   int64_t off) _Hide;
noasan int _Munmap(char *, size_t) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* ANSI */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_ */
