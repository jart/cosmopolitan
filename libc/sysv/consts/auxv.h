#ifndef COSMOPOLITAN_LIBC_CALLS_AUXV_H_
#define COSMOPOLITAN_LIBC_CALLS_AUXV_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const unsigned long AT_BASE;
extern const unsigned long AT_BASE_PLATFORM;
extern const unsigned long AT_CANARY;
extern const unsigned long AT_CANARYLEN;
extern const unsigned long AT_CLKTCK;
extern const unsigned long AT_DCACHEBSIZE;
extern const unsigned long AT_EGID;
extern const unsigned long AT_EHDRFLAGS;
extern const unsigned long AT_ENTRY;
extern const unsigned long AT_EUID;
extern const unsigned long AT_EXECFD;
extern const unsigned long AT_EXECFN;
extern const unsigned long AT_EXECPATH;
extern const unsigned long AT_FLAGS;
extern const unsigned long AT_GID;
extern const unsigned long AT_HWCAP2;
extern const unsigned long AT_HWCAP;
extern const unsigned long AT_ICACHEBSIZE;
extern const unsigned long AT_MINSIGSTKSZ;
extern const unsigned long AT_NCPUS;
extern const unsigned long AT_NOTELF;
extern const unsigned long AT_NO_AUTOMOUNT;
extern const unsigned long AT_OSRELDATE;
extern const unsigned long AT_PAGESIZES;
extern const unsigned long AT_PAGESIZESLEN;
extern const unsigned long AT_PAGESZ;
extern const unsigned long AT_PHDR;
extern const unsigned long AT_PHENT;
extern const unsigned long AT_PHNUM;
extern const unsigned long AT_PLATFORM;
extern const unsigned long AT_RANDOM;
extern const unsigned long AT_SECURE;
extern const unsigned long AT_STACKBASE;
extern const unsigned long AT_STACKPROT;
extern const unsigned long AT_SYSINFO_EHDR;
extern const unsigned long AT_TIMEKEEP;
extern const unsigned long AT_UCACHEBSIZE;
extern const unsigned long AT_UID;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define AT_BASE          SYMBOLIC(AT_BASE)
#define AT_BASE_PLATFORM SYMBOLIC(AT_BASE_PLATFORM)
#define AT_CANARY        SYMBOLIC(AT_CANARY)
#define AT_CANARYLEN     SYMBOLIC(AT_CANARYLEN)
#define AT_CLKTCK        SYMBOLIC(AT_CLKTCK)
#define AT_DCACHEBSIZE   SYMBOLIC(AT_DCACHEBSIZE)
#define AT_EGID          SYMBOLIC(AT_EGID)
#define AT_EHDRFLAGS     SYMBOLIC(AT_EHDRFLAGS)
#define AT_ENTRY         SYMBOLIC(AT_ENTRY)
#define AT_EUID          SYMBOLIC(AT_EUID)
#define AT_EXECFD        SYMBOLIC(AT_EXECFD)
#define AT_EXECFN        SYMBOLIC(AT_EXECFN)
#define AT_EXECPATH      SYMBOLIC(AT_EXECPATH)
#define AT_FLAGS         SYMBOLIC(AT_FLAGS)
#define AT_GID           SYMBOLIC(AT_GID)
#define AT_HWCAP         SYMBOLIC(AT_HWCAP)
#define AT_HWCAP2        SYMBOLIC(AT_HWCAP2)
#define AT_ICACHEBSIZE   SYMBOLIC(AT_ICACHEBSIZE)
#define AT_MINSIGSTKSZ   SYMBOLIC(AT_MINSIGSTKSZ)
#define AT_NCPUS         SYMBOLIC(AT_NCPUS)
#define AT_NOTELF        SYMBOLIC(AT_NOTELF)
#define AT_NO_AUTOMOUNT  SYMBOLIC(AT_NO_AUTOMOUNT)
#define AT_OSRELDATE     SYMBOLIC(AT_OSRELDATE)
#define AT_PAGESIZES     SYMBOLIC(AT_PAGESIZES)
#define AT_PAGESIZESLEN  SYMBOLIC(AT_PAGESIZESLEN)
#define AT_PAGESZ        SYMBOLIC(AT_PAGESZ)
#define AT_PHDR          SYMBOLIC(AT_PHDR)
#define AT_PHENT         SYMBOLIC(AT_PHENT)
#define AT_PHNUM         SYMBOLIC(AT_PHNUM)
#define AT_PLATFORM      SYMBOLIC(AT_PLATFORM)
#define AT_RANDOM        SYMBOLIC(AT_RANDOM)
#define AT_SECURE        SYMBOLIC(AT_SECURE)
#define AT_STACKBASE     SYMBOLIC(AT_STACKBASE)
#define AT_STACKPROT     SYMBOLIC(AT_STACKPROT)
#define AT_SYSINFO_EHDR  SYMBOLIC(AT_SYSINFO_EHDR)
#define AT_TIMEKEEP      SYMBOLIC(AT_TIMEKEEP)
#define AT_UCACHEBSIZE   SYMBOLIC(AT_UCACHEBSIZE)
#define AT_UID           SYMBOLIC(AT_UID)

#endif /* COSMOPOLITAN_LIBC_CALLS_AUXV_H_ */
