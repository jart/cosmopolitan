#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_FTW_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_FTW_H_
#include "libc/calls/struct/stat.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define FTW_F   1 /* file */
#define FTW_D   2 /* directory */
#define FTW_DNR 3 /* directory that cannot be read */
#define FTW_NS  4 /* not a symbolic link and stat failed */
#define FTW_SL  5 /* symbolic link */
#define FTW_DP  6 /* directory and FTW_DEPTH was specified */
#define FTW_SLN 7 /* symbolic link pointing to nonexistent file */

#define FTW_PHYS  1
#define FTW_MOUNT 2
#define FTW_CHDIR 4
#define FTW_DEPTH 8

struct FTW {
  int base;
  int level;
};

int ftw(const char *, int (*)(const char *, const struct stat *, int), int);
int nftw(const char *,
         int (*)(const char *, const struct stat *, int, struct FTW *), int,
         int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_FTW_H_ */
