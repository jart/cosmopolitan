#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_CACHESIZE_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_CACHESIZE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum CpuCacheType {
  kCpuCacheTypeData = 1,
  kCpuCacheTypeInstruction,
  kCpuCacheTypeUnified,
};

unsigned getcachesize(enum CpuCacheType, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_CACHESIZE_H_ */
