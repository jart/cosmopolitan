#ifndef COSMOPOLITAN_THIRD_PARTY_INFOZIP_ZIP_UNIX_ZIPUP_H_
#define COSMOPOLITAN_THIRD_PARTY_INFOZIP_ZIP_UNIX_ZIPUP_H_
#include "libc/calls/calls.h"
#include "libc/sysv/consts/o.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define fhow           O_RDONLY
#define fbad           (-1)
#define zopen(n, p)    open(n, p)
#define zread(f, b, n) read(f, b, n)
#define zclose(f)      close(f)
#define zerr(f)        (k == (extent)(-1L))
#define zstdin         0

typedef int ftype;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_INFOZIP_ZIP_UNIX_ZIPUP_H_ */
