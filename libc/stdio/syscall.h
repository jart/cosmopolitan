#ifndef COSMOPOLITAN_LIBC_STDIO_SYSCALL_H_
#define COSMOPOLITAN_LIBC_STDIO_SYSCALL_H_
COSMOPOLITAN_C_START_

#define SYS_gettid    186
#define SYS_getrandom 318

long syscall(long, ...) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_SYSCALL_H_ */
