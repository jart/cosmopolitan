#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_AT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_AT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview AT_xxx constants for fcntl(), fopenat(), etc..
 * @see libc/sysv/consts/auxv.h for getauxval() constants
 */

extern const int AT_FDCWD;
extern const int AT_SYMLINK_FOLLOW;
extern const int AT_SYMLINK_NOFOLLOW;
extern const int AT_REMOVEDIR;
extern const int AT_EACCESS;
extern const int AT_EMPTY_PATH;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define AT_FDCWD            SYMBOLIC(AT_FDCWD)
#define AT_SYMLINK_FOLLOW   SYMBOLIC(AT_SYMLINK_FOLLOW)
#define AT_SYMLINK_NOFOLLOW SYMBOLIC(AT_SYMLINK_NOFOLLOW)
#define AT_REMOVEDIR        SYMBOLIC(AT_REMOVEDIR)
#define AT_EACCESS          SYMBOLIC(AT_EACCESS)
#define AT_EMPTY_PATH       SYMBOLIC(AT_EMPTY_PATH)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_AT_H_ */
