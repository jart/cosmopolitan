#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <shaintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _SHAINTRIN_H_INCLUDED
#define _SHAINTRIN_H_INCLUDED

#ifndef __SHA__
#pragma GCC push_options
#pragma GCC target("sha")
#define __DISABLE_SHA__
#endif /* __SHA__ */

__funline __m128i _mm_sha1msg1_epu32(__m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_sha1msg1((__v4si)__A, (__v4si)__B);
}

__funline __m128i _mm_sha1msg2_epu32(__m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_sha1msg2((__v4si)__A, (__v4si)__B);
}

__funline __m128i _mm_sha1nexte_epu32(__m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_sha1nexte((__v4si)__A, (__v4si)__B);
}

#ifdef __OPTIMIZE__
__funline __m128i _mm_sha1rnds4_epu32(__m128i __A, __m128i __B, const int __I) {
  return (__m128i)__builtin_ia32_sha1rnds4((__v4si)__A, (__v4si)__B, __I);
}
#else
#define _mm_sha1rnds4_epu32(A, B, I)                                         \
  ((__m128i)__builtin_ia32_sha1rnds4((__v4si)(__m128i)A, (__v4si)(__m128i)B, \
                                     (int)I))
#endif

__funline __m128i _mm_sha256msg1_epu32(__m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_sha256msg1((__v4si)__A, (__v4si)__B);
}

__funline __m128i _mm_sha256msg2_epu32(__m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_sha256msg2((__v4si)__A, (__v4si)__B);
}

__funline __m128i _mm_sha256rnds2_epu32(__m128i __A, __m128i __B, __m128i __C) {
  return (__m128i)__builtin_ia32_sha256rnds2((__v4si)__A, (__v4si)__B,
                                             (__v4si)__C);
}

#ifdef __DISABLE_SHA__
#undef __DISABLE_SHA__
#pragma GCC pop_options
#endif /* __DISABLE_SHA__ */

#endif /* _SHAINTRIN_H_INCLUDED */
