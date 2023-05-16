#ifndef _X86INTRIN_H_INCLUDED
#error "Never use <ia32intrin.h> directly; include <x86intrin.h> instead."
#endif

__funline int __bsfd(int __X) {
  return __builtin_ctz(__X);
}

__funline int __bsrd(int __X) {
  return __builtin_ia32_bsrsi(__X);
}

__funline int __bswapd(int __X) {
  return __builtin_bswap32(__X);
}

#ifndef __iamcu__

#ifndef __SSE4_2__
#pragma GCC push_options
#pragma GCC target("sse4.2")
#define __DISABLE_SSE4_2__
#endif /* __SSE4_2__ */

__funline unsigned int __crc32b(unsigned int __C, unsigned char __V) {
  return __builtin_ia32_crc32qi(__C, __V);
}

__funline unsigned int __crc32w(unsigned int __C, unsigned short __V) {
  return __builtin_ia32_crc32hi(__C, __V);
}

__funline unsigned int __crc32d(unsigned int __C, unsigned int __V) {
  return __builtin_ia32_crc32si(__C, __V);
}

#ifdef __DISABLE_SSE4_2__
#undef __DISABLE_SSE4_2__
#pragma GCC pop_options
#endif /* __DISABLE_SSE4_2__ */

#endif /* __iamcu__ */

__funline int __popcntd(unsigned int __X) {
  return __builtin_popcount(__X);
}

#ifndef __iamcu__

__funline unsigned long long __rdpmc(int __S) {
  return __builtin_ia32_rdpmc(__S);
}

#endif /* __iamcu__ */

__funline unsigned long long __rdtsc(void) {
  return __builtin_ia32_rdtsc();
}

#ifndef __iamcu__

__funline unsigned long long __rdtscp(unsigned int *__A) {
  return __builtin_ia32_rdtscp(__A);
}

#endif /* __iamcu__ */

__funline unsigned char __rolb(unsigned char __X, int __C) {
  return __builtin_ia32_rolqi(__X, __C);
}

__funline unsigned short __rolw(unsigned short __X, int __C) {
  return __builtin_ia32_rolhi(__X, __C);
}

__funline unsigned int __rold(unsigned int __X, int __C) {
  __C &= 31;
  return (__X << __C) | (__X >> (-__C & 31));
}

__funline unsigned char __rorb(unsigned char __X, int __C) {
  return __builtin_ia32_rorqi(__X, __C);
}

__funline unsigned short __rorw(unsigned short __X, int __C) {
  return __builtin_ia32_rorhi(__X, __C);
}

__funline unsigned int __rord(unsigned int __X, int __C) {
  __C &= 31;
  return (__X >> __C) | (__X << (-__C & 31));
}

__funline void __pause(void) {
  __builtin_ia32_pause();
}

#ifdef __x86_64__

__funline int __bsfq(long long __X) {
  return __builtin_ctzll(__X);
}

__funline int __bsrq(long long __X) {
  return __builtin_ia32_bsrdi(__X);
}

__funline long long __bswapq(long long __X) {
  return __builtin_bswap64(__X);
}

#ifndef __SSE4_2__
#pragma GCC push_options
#pragma GCC target("sse4.2")
#define __DISABLE_SSE4_2__
#endif /* __SSE4_2__ */

__funline unsigned long long __crc32q(unsigned long long __C,
                                    unsigned long long __V) {
  return __builtin_ia32_crc32di(__C, __V);
}

#ifdef __DISABLE_SSE4_2__
#undef __DISABLE_SSE4_2__
#pragma GCC pop_options
#endif /* __DISABLE_SSE4_2__ */

__funline long long __popcntq(unsigned long long __X) {
  return __builtin_popcountll(__X);
}

__funline unsigned long long __rolq(unsigned long long __X, int __C) {
  __C &= 63;
  return (__X << __C) | (__X >> (-__C & 63));
}

__funline unsigned long long __rorq(unsigned long long __X, int __C) {
  __C &= 63;
  return (__X >> __C) | (__X << (-__C & 63));
}

__funline unsigned long long __readeflags(void) {
  return __builtin_ia32_readeflags_u64();
}

__funline void __writeeflags(unsigned long long __X) {
  __builtin_ia32_writeeflags_u64(__X);
}

#define _bswap64(a)  __bswapq(a)
#define _popcnt64(a) __popcntq(a)
#else

__funline unsigned int __readeflags(void) {
  return __builtin_ia32_readeflags_u32();
}

__funline void __writeeflags(unsigned int __X) {
  __builtin_ia32_writeeflags_u32(__X);
}

#endif

#ifdef __LP64__
#define _lrotl(a, b) __rolq((a), (b))
#define _lrotr(a, b) __rorq((a), (b))
#else
#define _lrotl(a, b) __rold((a), (b))
#define _lrotr(a, b) __rord((a), (b))
#endif

#define _bit_scan_forward(a) __bsfd(a)
#define _bit_scan_reverse(a) __bsrd(a)
#define _bswap(a)            __bswapd(a)
#define _popcnt32(a)         __popcntd(a)
#ifndef __iamcu__
#define _rdpmc(a)  __rdpmc(a)
#define _rdtscp(a) __rdtscp(a)
#endif /* __iamcu__ */
#define _rdtsc()     __rdtsc()
#define _rotwl(a, b) __rolw((a), (b))
#define _rotwr(a, b) __rorw((a), (b))
#define _rotl(a, b)  __rold((a), (b))
#define _rotr(a, b)  __rord((a), (b))
