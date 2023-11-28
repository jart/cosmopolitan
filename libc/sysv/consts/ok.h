#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_

#define F_OK 0
#define X_OK X_OK
#define W_OK W_OK
#define R_OK R_OK

COSMOPOLITAN_C_START_

extern const int X_OK;
extern const int W_OK;
extern const unsigned R_OK; /* warning: is sign bit on windows */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_ */
