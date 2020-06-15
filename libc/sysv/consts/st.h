#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_ST_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_ST_H_
#include "libc/runtime/symbolic.h"

#define ST_APPEND SYMBOLIC(ST_APPEND)
#define ST_IMMUTABLE SYMBOLIC(ST_IMMUTABLE)
#define ST_MANDLOCK SYMBOLIC(ST_MANDLOCK)
#define ST_NOATIME SYMBOLIC(ST_NOATIME)
#define ST_NODEV SYMBOLIC(ST_NODEV)
#define ST_NODIRATIME SYMBOLIC(ST_NODIRATIME)
#define ST_NOEXEC SYMBOLIC(ST_NOEXEC)
#define ST_NOSUID SYMBOLIC(ST_NOSUID)
#define ST_RDONLY SYMBOLIC(ST_RDONLY)
#define ST_RELATIME SYMBOLIC(ST_RELATIME)
#define ST_SYNCHRONOUS SYMBOLIC(ST_SYNCHRONOUS)
#define ST_WRITE SYMBOLIC(ST_WRITE)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long ST_APPEND;
hidden extern const long ST_IMMUTABLE;
hidden extern const long ST_MANDLOCK;
hidden extern const long ST_NOATIME;
hidden extern const long ST_NODEV;
hidden extern const long ST_NODIRATIME;
hidden extern const long ST_NOEXEC;
hidden extern const long ST_NOSUID;
hidden extern const long ST_RDONLY;
hidden extern const long ST_RELATIME;
hidden extern const long ST_SYNCHRONOUS;
hidden extern const long ST_WRITE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_ST_H_ */
