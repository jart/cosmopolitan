#ifndef COSMOPOLITAN_LIBC_INTRIN_PROMISES_H_
#define COSMOPOLITAN_LIBC_INTRIN_PROMISES_H_

#define PROMISE_DEFAULT    0
#define PROMISE_STDIO      1
#define PROMISE_RPATH      2
#define PROMISE_WPATH      3
#define PROMISE_CPATH      4
#define PROMISE_DPATH      5
#define PROMISE_FLOCK      6
#define PROMISE_FATTR      7
#define PROMISE_INET       8
#define PROMISE_UNIX       9
#define PROMISE_DNS        10
#define PROMISE_TTY        11
#define PROMISE_RECVFD     12
#define PROMISE_PROC       13
#define PROMISE_THREAD     14
#define PROMISE_EXEC       15
#define PROMISE_EXECNATIVE 16
#define PROMISE_ID         17
#define PROMISE_MAX        17

#define PLEDGED(x) (~__promises & (1L << PROMISE_##x))

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern unsigned long __promises;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PROMISES_H_ */
