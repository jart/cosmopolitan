#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long EX_CANTCREAT;
hidden extern const long EX_CONFIG;
hidden extern const long EX_DATAERR;
hidden extern const long EX_IOERR;
hidden extern const long EX_NOHOST;
hidden extern const long EX_NOINPUT;
hidden extern const long EX_NOPERM;
hidden extern const long EX_NOUSER;
hidden extern const long EX_OK;
hidden extern const long EX_OSERR;
hidden extern const long EX_OSFILE;
hidden extern const long EX_PROTOCOL;
hidden extern const long EX_SOFTWARE;
hidden extern const long EX_TEMPFAIL;
hidden extern const long EX_UNAVAILABLE;
hidden extern const long EX_USAGE;
hidden extern const long EX__BASE;
hidden extern const long EX__MAX;

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
