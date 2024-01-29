#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_X86COMPILER_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_X86COMPILER_H_
#ifdef __x86_64__

/**
 * @fileoverview x86 cpu feature compile-time requirement detection.
 * @see -march=native, -mavx2, etc.
 */

#ifdef __AES__
#define _X86_CC_AES 1
#else
#define _X86_CC_AES 0
#endif

#ifdef __AVX__
#define _X86_CC_AVX 1
#else
#define _X86_CC_AVX 0
#endif

#ifdef __AVX2__
#define _X86_CC_AVX2 1
#else
#define _X86_CC_AVX2 0
#endif

#ifdef __F16C__
#define _X86_CC_F16C 1
#else
#define _X86_CC_F16C 0
#endif

#ifdef __AVXVNNI__
#define _X86_CC_AVXVNNI 1
#else
#define _X86_CC_AVXVNNI 0
#endif

#ifdef __AVXVNNIINT8__
#define _X86_CC_AVXVNNIINT8 1
#else
#define _X86_CC_AVXVNNIINT8 0
#endif

#ifdef __AVXVNNIINT16__
#define _X86_CC_AVXVNNIINT16 1
#else
#define _X86_CC_AVXVNNIINT16 0
#endif

#ifdef __AVX512F__
#define _X86_CC_AVX512F 1
#else
#define _X86_CC_AVX512F 0
#endif

#ifdef __AVX512BF16__
#define _X86_CC_AVX512BF16 1
#else
#define _X86_CC_AVX512BF16 0
#endif

#ifdef __AVX512VBMI__
#define _X86_CC_AVX512VBMI 1
#else
#define _X86_CC_AVX512VBMI 0
#endif

#ifdef __AVX512VNNI__
#define _X86_CC_AVX512VNNI 1
#else
#define _X86_CC_AVX512VNNI 0
#endif

#ifdef __AVX5124VNNIW__
#define _X86_CC_AVX5124VNNIW 1
#else
#define _X86_CC_AVX5124VNNIW 0
#endif

#ifdef __ABM__
#define _X86_CC_ABM 1
#else
#define _X86_CC_ABM 0
#endif

#ifdef __BMI__
#define _X86_CC_BMI 1
#else
#define _X86_CC_BMI 0
#endif

#ifdef __BMI2__
#define _X86_CC_BMI2 1
#else
#define _X86_CC_BMI2 0
#endif

#ifdef __FMA__
#define _X86_CC_FMA 1
#else
#define _X86_CC_FMA 0
#endif

#ifdef __ADX__
#define _X86_CC_ADX 1
#else
#define _X86_CC_ADX 0
#endif

#ifdef __PCLMUL__
#define _X86_CC_PCLMUL 1
#else
#define _X86_CC_PCLMUL 0
#endif

#ifdef __POPCNT__
#define _X86_CC_POPCNT 1
#else
#define _X86_CC_POPCNT 0
#endif

#ifdef __RDRND__
#define _X86_CC_RDRND 1
#else
#define _X86_CC_RDRND 0
#endif

#ifdef __RDSEED__
#define _X86_CC_RDSEED 1
#else
#define _X86_CC_RDSEED 0
#endif

#ifdef __SHA__
#define _X86_CC_SHA 1
#else
#define _X86_CC_SHA 0
#endif

#ifdef __SSSE3__
#define _X86_CC_SSSE3 1
#else
#define _X86_CC_SSSE3 0
#endif

#ifdef __SSE__
#define _X86_CC_SSE 1
#else
#define _X86_CC_SSE 0
#endif

#ifdef __SSE2__
#define _X86_CC_SSE2 1
#else
#define _X86_CC_SSE2 0
#endif

#ifdef __SSE3__
#define _X86_CC_SSE3 1
#else
#define _X86_CC_SSE3 0
#endif

#ifdef __SSE4_1__
#define _X86_CC_SSE4_1 1
#else
#define _X86_CC_SSE4_1 0
#endif

#ifdef __SSE4_2__
#define _X86_CC_SSE4_2 1
#else
#define _X86_CC_SSE4_2 0
#endif

#ifdef __XSAVE__
#define _X86_CC_XSAVE 1
#else
#define _X86_CC_XSAVE 0
#endif

#ifdef __CLFLUSHOPT__
#define _X86_CC_CLFLUSHOPT 1
#else
#define _X86_CC_CLFLUSHOPT 0
#endif

#ifdef __RDPID__
#define _X86_CC_RDPID 1
#else
#define _X86_CC_RDPID 0
#endif

#endif /* __x86_64__ */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_X86COMPILER_H_ */
