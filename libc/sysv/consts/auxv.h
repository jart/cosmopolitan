#ifndef COSMOPOLITAN_LIBC_CALLS_AUXV_H_
#define COSMOPOLITAN_LIBC_CALLS_AUXV_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long AT_BASE;
hidden extern const long AT_BASE_PLATFORM;
hidden extern const long AT_CLKTCK;
hidden extern const long AT_DCACHEBSIZE;
hidden extern const long AT_EGID;
hidden extern const long AT_ENTRY;
hidden extern const long AT_EUID;
hidden extern const long AT_EXECFD;
hidden extern const long AT_EXECFN;
hidden extern const long AT_GID;
hidden extern const long AT_ICACHEBSIZE;
hidden extern const long AT_NOTELF;
hidden extern const long AT_NO_AUTOMOUNT;
hidden extern const long AT_OSRELDATE;
hidden extern const long AT_PAGESZ;
hidden extern const long AT_PHDR;
hidden extern const long AT_PHENT;
hidden extern const long AT_PHNUM;
hidden extern const long AT_PLATFORM;
hidden extern const long AT_RANDOM;
hidden extern const long AT_SECURE;
hidden extern const long AT_SYSINFO_EHDR;
hidden extern const long AT_UCACHEBSIZE;
hidden extern const long AT_UID;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define AT_BASE          SYMBOLIC(AT_BASE)
#define AT_BASE_PLATFORM SYMBOLIC(AT_BASE_PLATFORM)
#define AT_CLKTCK        SYMBOLIC(AT_CLKTCK)
#define AT_DCACHEBSIZE   SYMBOLIC(AT_DCACHEBSIZE)
#define AT_EGID          SYMBOLIC(AT_EGID)
#define AT_ENTRY         SYMBOLIC(AT_ENTRY)
#define AT_EUID          SYMBOLIC(AT_EUID)
#define AT_EXECFD        SYMBOLIC(AT_EXECFD)
#define AT_EXECFN        SYMBOLIC(AT_EXECFN)
#define AT_GID           SYMBOLIC(AT_GID)
#define AT_ICACHEBSIZE   SYMBOLIC(AT_ICACHEBSIZE)
#define AT_NOTELF        SYMBOLIC(AT_NOTELF)
#define AT_NO_AUTOMOUNT  SYMBOLIC(AT_NO_AUTOMOUNT)
#define AT_OSRELDATE     SYMBOLIC(AT_OSRELDATE)
#define AT_PAGESZ        SYMBOLIC(AT_PAGESZ)
#define AT_PHDR          SYMBOLIC(AT_PHDR)
#define AT_PHENT         SYMBOLIC(AT_PHENT)
#define AT_PHNUM         SYMBOLIC(AT_PHNUM)
#define AT_PLATFORM      SYMBOLIC(AT_PLATFORM)
#define AT_RANDOM        SYMBOLIC(AT_RANDOM)
#define AT_SECURE        SYMBOLIC(AT_SECURE)
#define AT_SYSINFO_EHDR  SYMBOLIC(AT_SYSINFO_EHDR)
#define AT_UCACHEBSIZE   SYMBOLIC(AT_UCACHEBSIZE)
#define AT_UID           SYMBOLIC(AT_UID)

#endif /* COSMOPOLITAN_LIBC_CALLS_AUXV_H_ */
