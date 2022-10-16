#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_

/**
 * @fileoverview Eric Allman's exit() codes
 *
 * - Broadly supported style guideline;
 * - Dating back to 1980 in 4.0BSD;
 * - That won't be standardized.
 *
 */

#define EX_CANTCREAT   73
#define EX_CONFIG      78
#define EX_DATAERR     65
#define EX_IOERR       74
#define EX_NOHOST      68
#define EX_NOINPUT     66
#define EX_NOPERM      77
#define EX_NOUSER      67
#define EX_OK          0
#define EX_OSERR       71
#define EX_OSFILE      72
#define EX_PROTOCOL    76
#define EX_SOFTWARE    70
#define EX_TEMPFAIL    75
#define EX_UNAVAILABLE 69
#define EX_USAGE       64
#define EX__BASE       64
#define EX__MAX        78

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_EX_H_ */
