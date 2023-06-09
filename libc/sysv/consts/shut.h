#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SHUT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SHUT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int SHUT_RD;
extern const int SHUT_RDWR;
extern const int SHUT_WR;

#define SHUT_RD   0
#define SHUT_RDWR 2
#define SHUT_WR   1

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SHUT_H_ */
