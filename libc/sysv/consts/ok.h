#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_

#define F_OK F_OK
#define R_OK R_OK
#define W_OK W_OK
#define X_OK X_OK

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int F_OK;
extern const int R_OK;
extern const int W_OK;
extern const int X_OK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_ */
