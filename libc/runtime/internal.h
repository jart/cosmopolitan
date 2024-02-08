#ifndef COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/dce.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/runtime/runtime.h"

#define STACK_CEIL 0x700000000000ul
#define STACK_SIZE FRAMESIZE

#define RUNLEVEL_MALLOC 1

COSMOPOLITAN_C_START_

extern int __pid;
extern char __runlevel;
extern int ftrace_stackdigs;
extern const signed char kNtStdio[3];
extern const char v_ntsubsystem[] __attribute__((__weak__));
extern const uintptr_t __fini_array_end[] __attribute__((__weak__));
extern const uintptr_t __fini_array_start[] __attribute__((__weak__));

extern unsigned char _tdata_start[] __attribute__((__weak__));
extern unsigned char _tdata_end[] __attribute__((__weak__));
extern unsigned char _tdata_size[] __attribute__((__weak__));
extern unsigned char _tbss_start[] __attribute__((__weak__));
extern unsigned char _tbss_end[] __attribute__((__weak__));
extern unsigned char _tbss_size[] __attribute__((__weak__));
extern unsigned char _tbss_offset[] __attribute__((__weak__));
extern unsigned char _tls_size[] __attribute__((__weak__));
extern unsigned char _tls_content[] __attribute__((__weak__));

void _init(void);
int ftrace_init(void);
void ftrace_hook(void);
void __morph_tls(void);
void __enable_tls(void);
void __stack_chk_fail(void) wontreturn relegated;
void __stack_chk_fail_local(void) wontreturn relegated;
long _setstack(void *, void *, ...);
int GetDosArgv(const char16_t *, char *, size_t, char **, size_t);
int GetDosEnviron(const char16_t *, char *, size_t, char **, size_t);
bool __intercept_flag(int *, char *[], const char *);
int sys_mprotect_nt(void *, size_t, int);
int __inflate(void *, size_t, const void *, size_t);
void *__mmap_unlocked(void *, size_t, int, int, int, int64_t);
int __munmap_unlocked(char *, size_t);
void __on_arithmetic_overflow(void);
void __init_fds(int, char **, char **);
void __init_program_executable_name(void);

COSMOPOLITAN_C_END_
#endif /* ANSI */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_INTERNAL_H_ */
