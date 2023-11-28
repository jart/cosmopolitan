#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_INTERNAL_H_
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"
COSMOPOLITAN_C_START_

#if __AVX__ || __AVX2__ || __AVX512F__
// horizontally add 8 floats
static inline float hsum_float_8(const __m256 x) {
  __m128 res = _mm256_extractf128_ps(x, 1);
  res = _mm_add_ps(res, _mm256_castps256_ps128(x));
  res = _mm_add_ps(res, _mm_movehl_ps(res, res));
  res = _mm_add_ss(res, _mm_movehdup_ps(res));
  return _mm_cvtss_f32(res);
}
#endif

#if __AVX2__ || __AVX512F__

// spread 32 bits to 32 bytes { 0x00, 0xFF }
static inline __m256i bytes_from_bits_32(const uint8_t *x) {
  uint32_t x32;
  memcpy(&x32, x, sizeof(uint32_t));
  const __m256i shuf_mask =
      _mm256_set_epi64x(0x0303030303030303, 0x0202020202020202,
                        0x0101010101010101, 0x0000000000000000);
  __m256i bytes = _mm256_shuffle_epi8(_mm256_set1_epi32(x32), shuf_mask);
  const __m256i bit_mask = _mm256_set1_epi64x(0x7fbfdfeff7fbfdfe);
  bytes = _mm256_or_si256(bytes, bit_mask);
  return _mm256_cmpeq_epi8(bytes, _mm256_set1_epi64x(-1));
}

// add int16_t pairwise and return as float vector
static inline __m256 sum_i16_pairs_float(const __m256i x) {
  const __m256i ones = _mm256_set1_epi16(1);
  const __m256i summed_pairs = _mm256_madd_epi16(ones, x);
  return _mm256_cvtepi32_ps(summed_pairs);
}

static inline __m256 mul_sum_us8_pairs_float(const __m256i ax,
                                             const __m256i sy) {
#if __AVXVNNI__
  const __m256i zero = _mm256_setzero_si256();
  const __m256i summed_pairs = _mm256_dpbusd_epi32(zero, ax, sy);
  return _mm256_cvtepi32_ps(summed_pairs);
#else
  // Perform multiplication and create 16-bit values
  const __m256i dot = _mm256_maddubs_epi16(ax, sy);
  return sum_i16_pairs_float(dot);
#endif
}

// multiply int8_t, add results pairwise twice and return as float vector
static inline __m256 mul_sum_i8_pairs_float(const __m256i x, const __m256i y) {
#if __AVXVNNIINT8__
  const __m256i zero = _mm256_setzero_si256();
  const __m256i summed_pairs = _mm256_dpbssd_epi32(zero, x, y);
  return _mm256_cvtepi32_ps(summed_pairs);
#else
  // Get absolute values of x vectors
  const __m256i ax = _mm256_sign_epi8(x, x);
  // Sign the values of the y vectors
  const __m256i sy = _mm256_sign_epi8(y, x);
  return mul_sum_us8_pairs_float(ax, sy);
#endif
}

static inline __m256i bytes_from_nibbles_32(const uint8_t *rsi) {
  const __m128i tmp = _mm_loadu_si128((const __m128i *)rsi);
  const __m256i bytes = _mm256_set_m128i(_mm_srli_epi16(tmp, 4), tmp);
  const __m256i lowMask = _mm256_set1_epi8(0xF);
  return _mm256_and_si256(lowMask, bytes);
}

#elif defined(__AVX__)

// spread 32 bits to 32 bytes { 0x00, 0xFF }
static inline __m256i bytes_from_bits_32(const uint8_t *x) {
  uint32_t x32;
  memcpy(&x32, x, sizeof(uint32_t));
  const __m128i shuf_maskl =
      _mm_set_epi64x(0x0101010101010101, 0x0000000000000000);
  const __m128i shuf_maskh =
      _mm_set_epi64x(0x0303030303030303, 0x0202020202020202);
  __m128i bytesl = _mm_shuffle_epi8(_mm_set1_epi32(x32), shuf_maskl);
  __m128i bytesh = _mm_shuffle_epi8(_mm_set1_epi32(x32), shuf_maskh);
  const __m128i bit_mask = _mm_set1_epi64x(0x7fbfdfeff7fbfdfe);
  bytesl = _mm_or_si128(bytesl, bit_mask);
  bytesh = _mm_or_si128(bytesh, bit_mask);
  bytesl = _mm_cmpeq_epi8(bytesl, _mm_set1_epi64x(-1));
  bytesh = _mm_cmpeq_epi8(bytesh, _mm_set1_epi64x(-1));
  return _mm256_set_m128i(bytesh, bytesl);
}

// add int16_t pairwise and return as float vector
static inline __m256 sum_i16_pairs_float(const __m128i xh, const __m128i xl) {
  const __m128i ones = _mm_set1_epi16(1);
  const __m128i summed_pairsl = _mm_madd_epi16(ones, xl);
  const __m128i summed_pairsh = _mm_madd_epi16(ones, xh);
  const __m256i summed_pairs = _mm256_set_m128i(summed_pairsh, summed_pairsl);
  return _mm256_cvtepi32_ps(summed_pairs);
}

static inline __m256 mul_sum_us8_pairs_float(const __m256i ax,
                                             const __m256i sy) {
  const __m128i axl = _mm256_castsi256_si128(ax);
  const __m128i axh = _mm256_extractf128_si256(ax, 1);
  const __m128i syl = _mm256_castsi256_si128(sy);
  const __m128i syh = _mm256_extractf128_si256(sy, 1);
  // Perform multiplication and create 16-bit values
  const __m128i dotl = _mm_maddubs_epi16(axl, syl);
  const __m128i doth = _mm_maddubs_epi16(axh, syh);
  return sum_i16_pairs_float(doth, dotl);
}

// multiply int8_t, add results pairwise twice and return as float vector
static inline __m256 mul_sum_i8_pairs_float(const __m256i x, const __m256i y) {
  const __m128i xl = _mm256_castsi256_si128(x);
  const __m128i xh = _mm256_extractf128_si256(x, 1);
  const __m128i yl = _mm256_castsi256_si128(y);
  const __m128i yh = _mm256_extractf128_si256(y, 1);
  // Get absolute values of x vectors
  const __m128i axl = _mm_sign_epi8(xl, xl);
  const __m128i axh = _mm_sign_epi8(xh, xh);
  // Sign the values of the y vectors
  const __m128i syl = _mm_sign_epi8(yl, xl);
  const __m128i syh = _mm_sign_epi8(yh, xh);
  // Perform multiplication and create 16-bit values
  const __m128i dotl = _mm_maddubs_epi16(axl, syl);
  const __m128i doth = _mm_maddubs_epi16(axh, syh);
  return sum_i16_pairs_float(doth, dotl);
}

// Unpack 32 4-bit fields into 32 bytes
// The output vector contains 32 bytes, each one in [ 0 .. 15 ] interval
static inline __m256i bytes_from_nibbles_32(const uint8_t *rsi) {
  // Load 16 bytes from memory
  __m128i tmpl = _mm_loadu_si128((const __m128i *)rsi);
  __m128i tmph = _mm_srli_epi16(tmpl, 4);
  const __m128i lowMask = _mm_set1_epi8(0xF);
  tmpl = _mm_and_si128(lowMask, tmpl);
  tmph = _mm_and_si128(lowMask, tmph);
  return _mm256_set_m128i(tmph, tmpl);
}

#endif /* AVX */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_INTERNAL_H_ */
