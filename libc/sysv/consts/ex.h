#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int EX_CANTCREAT;
extern const int EX_CONFIG;
extern const int EX_DATAERR;
extern const int EX_IOERR;
extern const int EX_NOHOST;
extern const int EX_NOINPUT;
extern const int EX_NOPERM;
extern const int EX_NOUSER;
extern const int EX_OK;
extern const int EX_OSERR;
extern const int EX_OSFILE;
extern const int EX_PROTOCOL;
extern const int EX_SOFTWARE;
extern const int EX_TEMPFAIL;
extern const int EX_UNAVAILABLE;
extern const int EX_USAGE;
extern const int EX__BASE;
extern const int EX__MAX;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define EX_CANTCREAT LITERALLY(73)
#define EX_CONFIG LITERALLY(78)
#define EX_DATAERR LITERALLY(65)
#define EX_IOERR LITERALLY(74)
#define EX_NOHOST LITERALLY(68)
#define EX_NOINPUT LITERALLY(66)
#define EX_NOPERM LITERALLY(77)
#define EX_NOUSER LITERALLY(67)
#define EX_OK LITERALLY(0)
#define EX_OSERR LITERALLY(71)
#define EX_OSFILE LITERALLY(72)
#define EX_PROTOCOL LITERALLY(76)
#define EX_SOFTWARE LITERALLY(70)
#define EX_TEMPFAIL LITERALLY(75)
#define EX_UNAVAILABLE LITERALLY(69)
#define EX_USAGE LITERALLY(64)
#define EX__BASE LITERALLY(64)
#define EX__MAX LITERALLY(78)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_ */
