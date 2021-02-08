#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_C_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_C_H_
#include "libc/runtime/symbolic.h"

#define C_IRGRP SYMBOLIC(C_IRGRP)
#define C_IROTH SYMBOLIC(C_IROTH)
#define C_IRUSR SYMBOLIC(C_IRUSR)
#define C_ISBLK SYMBOLIC(C_ISBLK)
#define C_ISCHR SYMBOLIC(C_ISCHR)
#define C_ISCTG SYMBOLIC(C_ISCTG)
#define C_ISDIR SYMBOLIC(C_ISDIR)
#define C_ISFIFO SYMBOLIC(C_ISFIFO)
#define C_ISGID SYMBOLIC(C_ISGID)
#define C_ISLNK SYMBOLIC(C_ISLNK)
#define C_ISREG SYMBOLIC(C_ISREG)
#define C_ISSOCK SYMBOLIC(C_ISSOCK)
#define C_ISUID SYMBOLIC(C_ISUID)
#define C_ISVTX SYMBOLIC(C_ISVTX)
#define C_IWGRP SYMBOLIC(C_IWGRP)
#define C_IWOTH SYMBOLIC(C_IWOTH)
#define C_IWUSR SYMBOLIC(C_IWUSR)
#define C_IXGRP SYMBOLIC(C_IXGRP)
#define C_IXOTH SYMBOLIC(C_IXOTH)
#define C_IXUSR SYMBOLIC(C_IXUSR)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long C_IRGRP;
extern const long C_IROTH;
extern const long C_IRUSR;
extern const long C_ISBLK;
extern const long C_ISCHR;
extern const long C_ISCTG;
extern const long C_ISDIR;
extern const long C_ISFIFO;
extern const long C_ISGID;
extern const long C_ISLNK;
extern const long C_ISREG;
extern const long C_ISSOCK;
extern const long C_ISUID;
extern const long C_ISVTX;
extern const long C_IWGRP;
extern const long C_IWOTH;
extern const long C_IWUSR;
extern const long C_IXGRP;
extern const long C_IXOTH;
extern const long C_IXUSR;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_C_H_ */
