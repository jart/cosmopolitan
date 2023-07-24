#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_CPU_FEATURES_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_CPU_FEATURES_H_
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#ifdef __x86_64__

#define x86_cpu_enable_sse2   X86_HAVE(SSE2)
#define x86_cpu_enable_ssse3  X86_HAVE(SSSE3)
#define x86_cpu_enable_simd   (X86_HAVE(SSE4_2) && X86_HAVE(PCLMUL))
#define x86_cpu_enable_avx512 X86_HAVE(AVX512F)
#define cpu_check_features()  (void)0

#elif defined(__aarch64__)

#define cpu_check_features zlib_cpu_check_features
extern int arm_cpu_enable_crc32;
extern int arm_cpu_enable_pmull;
void cpu_check_features(void);

#endif
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_CPU_FEATURES_H_ */
