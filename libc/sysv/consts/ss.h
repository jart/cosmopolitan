#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int SS_DISABLE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SIGSTKSZ    32768
#define MINSIGSTKSZ 32768 /* xnu defines the highest minimum */
#define SS_ONSTACK  1
#define SS_DISABLE  SS_DISABLE

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SS_H_ */
