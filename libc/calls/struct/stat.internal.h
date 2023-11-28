#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_INTERNAL_H_
#include "libc/calls/struct/stat.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

int sys_fstat(int, struct stat *);
int sys_fstatat(int, const char *, struct stat *, int);
int sys_fstat_nt(int, struct stat *);
int sys_fstat_nt_special(int, struct stat *);
int sys_fstat_nt_handle(int64_t, const char16_t *, struct stat *);
int sys_fstatat_nt(int, const char *, struct stat *, int);
int sys_lstat_nt(const char *, struct stat *);
int sys_fstat_metal(int, struct stat *);

const char *DescribeStat(char[300], int, const struct stat *);
#define DescribeStat(rc, st) DescribeStat(alloca(300), rc, st)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_INTERNAL_H_ */
