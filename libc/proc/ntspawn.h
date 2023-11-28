#ifndef COSMOPOLITAN_NTSPAWN_H_
#define COSMOPOLITAN_NTSPAWN_H_
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/startupinfo.h"
COSMOPOLITAN_C_START_

void mungentpath(char *);
int mkntcmdline(char16_t[32767], char *const[]);
int mkntenvblock(char16_t[32767], char *const[], char *const[], char[32767]);
int ntspawn(int64_t, const char *, char *const[], char *const[], char *const[],
            uint32_t, const char16_t *, int64_t, int64_t *, uint32_t,
            const struct NtStartupInfo *, struct NtProcessInformation *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NTSPAWN_H_ */
