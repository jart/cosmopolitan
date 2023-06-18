#ifndef COSMOPOLITAN_LIBC_CALLS_AUXV_H_
#define COSMOPOLITAN_LIBC_CALLS_AUXV_H_
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

#define AT_BASE          AT_BASE
#define AT_BASE_PLATFORM AT_BASE_PLATFORM
#define AT_CANARY        AT_CANARY
#define AT_CANARYLEN     AT_CANARYLEN
#define AT_CLKTCK        AT_CLKTCK
#define AT_DCACHEBSIZE   AT_DCACHEBSIZE
#define AT_EGID          AT_EGID
#define AT_EHDRFLAGS     AT_EHDRFLAGS
#define AT_ENTRY         AT_ENTRY
#define AT_EUID          AT_EUID
#define AT_EXECFD        AT_EXECFD
#define AT_EXECFN        AT_EXECFN
#define AT_EXECPATH      AT_EXECPATH
#define AT_FLAGS         AT_FLAGS
#define AT_GID           AT_GID
#define AT_HWCAP         AT_HWCAP
#define AT_HWCAP2        AT_HWCAP2
#define AT_ICACHEBSIZE   AT_ICACHEBSIZE
#define AT_MINSIGSTKSZ   AT_MINSIGSTKSZ
#define AT_NCPUS         AT_NCPUS
#define AT_NOTELF        AT_NOTELF
#define AT_NO_AUTOMOUNT  AT_NO_AUTOMOUNT
#define AT_OSRELDATE     AT_OSRELDATE
#define AT_PAGESIZES     AT_PAGESIZES
#define AT_PAGESIZESLEN  AT_PAGESIZESLEN
#define AT_PAGESZ        AT_PAGESZ
#define AT_PHDR          AT_PHDR
#define AT_PHENT         AT_PHENT
#define AT_PHNUM         AT_PHNUM
#define AT_PLATFORM      AT_PLATFORM
#define AT_RANDOM        AT_RANDOM
#define AT_SECURE        AT_SECURE
#define AT_STACKBASE     AT_STACKBASE
#define AT_STACKPROT     AT_STACKPROT
#define AT_SYSINFO_EHDR  AT_SYSINFO_EHDR
#define AT_TIMEKEEP      AT_TIMEKEEP
#define AT_UCACHEBSIZE   AT_UCACHEBSIZE
#define AT_UID           AT_UID


#endif /* COSMOPOLITAN_LIBC_CALLS_AUXV_H_ */
