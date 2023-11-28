#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_CACHESIZE_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_CACHESIZE_H_

#define kCpuCacheTypeData        1
#define kCpuCacheTypeInstruction 2
#define kCpuCacheTypeUnified     3

COSMOPOLITAN_C_START_

unsigned _getcachesize(int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_CACHESIZE_H_ */
