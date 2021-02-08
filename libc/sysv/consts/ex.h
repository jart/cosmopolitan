#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long EX_CANTCREAT;
extern const long EX_CONFIG;
extern const long EX_DATAERR;
extern const long EX_IOERR;
extern const long EX_NOHOST;
extern const long EX_NOINPUT;
extern const long EX_NOPERM;
extern const long EX_NOUSER;
extern const long EX_OK;
extern const long EX_OSERR;
extern const long EX_OSFILE;
extern const long EX_PROTOCOL;
extern const long EX_SOFTWARE;
extern const long EX_TEMPFAIL;
extern const long EX_UNAVAILABLE;
extern const long EX_USAGE;
extern const long EX__BASE;
extern const long EX__MAX;

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
