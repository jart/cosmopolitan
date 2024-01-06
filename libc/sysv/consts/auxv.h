#ifndef COSMOPOLITAN_LIBC_CALLS_AUXV_H_
#define COSMOPOLITAN_LIBC_CALLS_AUXV_H_

/*
 * integral getauxval() keys
 */
#define AT_PHDR                     3
#define AT_PHENT                    4
#define AT_PHNUM                    5
#define AT_PAGESZ                   6
#define AT_BASE                     7
#define AT_FLAGS                    8
#define AT_FLAGS_PRESERVE_ARGV0_BIT 0
#define AT_FLAGS_PRESERVE_ARGV0     (1 << AT_FLAGS_PRESERVE_ARGV0_BIT)
#define AT_ENTRY                    9

COSMOPOLITAN_C_START_

/*
 * portable getauxval() keys
 */
extern const unsigned long AT_EXECFN;
extern const unsigned long AT_SECURE;
extern const unsigned long AT_RANDOM;
extern const unsigned long AT_HWCAP;
extern const unsigned long AT_HWCAP2;
extern const unsigned long AT_UID;
extern const unsigned long AT_EUID;
extern const unsigned long AT_GID;
extern const unsigned long AT_EGID;
#define AT_EXECFN AT_EXECFN
#define AT_SECURE AT_SECURE
#define AT_RANDOM AT_RANDOM
#define AT_HWCAP  AT_HWCAP
#define AT_HWCAP2 AT_HWCAP2
#define AT_UID    AT_UID
#define AT_EUID   AT_EUID
#define AT_GID    AT_GID
#define AT_EGID   AT_EGID

/*
 * platform-specific getauxval() keys
 */
extern const unsigned long AT_BASE_PLATFORM;
extern const unsigned long AT_CANARY;
extern const unsigned long AT_CANARYLEN;
extern const unsigned long AT_CLKTCK;
extern const unsigned long AT_DCACHEBSIZE;
extern const unsigned long AT_EHDRFLAGS;
extern const unsigned long AT_EXECFD;
extern const unsigned long AT_EXECPATH;
extern const unsigned long AT_ICACHEBSIZE;
extern const unsigned long AT_MINSIGSTKSZ;
extern const unsigned long AT_NCPUS;
extern const unsigned long AT_NOTELF;
extern const unsigned long AT_NO_AUTOMOUNT;
extern const unsigned long AT_OSRELDATE;
extern const unsigned long AT_PAGESIZES;
extern const unsigned long AT_PAGESIZESLEN;
extern const unsigned long AT_PLATFORM;
extern const unsigned long AT_STACKBASE;
extern const unsigned long AT_STACKPROT;
extern const unsigned long AT_SYSINFO_EHDR;
extern const unsigned long AT_TIMEKEEP;
extern const unsigned long AT_UCACHEBSIZE;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_AUXV_H_ */
