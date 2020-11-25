#ifndef THIRD_PARTY_ZLIB_CHUNKCOPY_H
#define THIRD_PARTY_ZLIB_CHUNKCOPY_H
#include "libc/bits/emmintrin.internal.h"
#include "third_party/zlib/zutil.h"

asm(".ident\t\"\\n\\n\
Chromium (BSD-3 License)\\n\
Copyright 2017 The Chromium Authors\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * @fileoverview fast chunk copy and set operations
 *
 * The chunk-copy code above deals with writing the decoded DEFLATE data
 * to the output with SIMD methods to increase decode speed. Reading the
 * input to the DEFLATE decoder with a wide, SIMD method can also
 * increase decode speed. This option is supported on little endian
 * machines, and reads the input data in 64-bit (8 byte) chunks.
 */

#define Z_BUILTIN_MEMCPY              __builtin_memcpy
#define Z_RESTRICT                    restrict
#define Z_STATIC_ASSERT(name, assert) typedef char name[(assert) ? 1 : -1]

#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef long long z_vec128i_t _Vector_size(16);

/*
 * chunk copy type: the z_vec128i_t type size should be exactly 128-bits
 * and equal to CHUNKCOPY_CHUNK_SIZE.
 */
#define CHUNKCOPY_CHUNK_SIZE sizeof(z_vec128i_t)

Z_STATIC_ASSERT(vector_128_bits_wide,
                CHUNKCOPY_CHUNK_SIZE == sizeof(int8_t) * 16);

/**
 * Ask the compiler to perform a wide, unaligned load with a machine
 * instruction appropriate for the z_vec128i_t type.
 */
static inline z_vec128i_t loadchunk(const unsigned char *s) {
  z_vec128i_t v;
  Z_BUILTIN_MEMCPY(&v, s, sizeof(v));
  return v;
}

/**
 * Ask the compiler to perform a wide, unaligned store with a machine
 * instruction appropriate for the z_vec128i_t type.
 */
static inline void storechunk(unsigned char *d, const z_vec128i_t v) {
  Z_BUILTIN_MEMCPY(d, &v, sizeof(v));
}

/**
 * Perform a memcpy-like operation, assuming that length is non-zero and
 * that it's OK to overwrite at least CHUNKCOPY_CHUNK_SIZE bytes of
 * output even if the length is shorter than this.
 *
 * It also guarantees that it will properly unroll the data if the distance
 * between `out` and `from` is at least CHUNKCOPY_CHUNK_SIZE, which we rely on
 * in chunkcopy_relaxed().
 *
 * Aside from better memory bus utilisation, this means that short copies
 * (CHUNKCOPY_CHUNK_SIZE bytes or fewer) will fall straight through the loop
 * without iteration, which will hopefully make the branch prediction more
 * reliable.
 */
static inline unsigned char *chunkcopy_core(unsigned char *out,
                                            const unsigned char *from,
                                            unsigned len) {
  const int bump = (--len % CHUNKCOPY_CHUNK_SIZE) + 1;
  storechunk(out, loadchunk(from));
  out += bump;
  from += bump;
  len /= CHUNKCOPY_CHUNK_SIZE;
  while (len-- > 0) {
    storechunk(out, loadchunk(from));
    out += CHUNKCOPY_CHUNK_SIZE;
    from += CHUNKCOPY_CHUNK_SIZE;
  }
  return out;
}

/**
 * Like chunkcopy_core(), but avoid writing beyond of legal output.
 *
 * Accepts an additional pointer to the end of safe output.  A generic safe
 * copy would use (out + len), but it's normally the case that the end of the
 * output buffer is beyond the end of the current copy, and this can still be
 * exploited.
 */
static inline unsigned char *chunkcopy_core_safe(unsigned char *out,
                                                 const unsigned char *from,
                                                 unsigned len,
                                                 unsigned char *limit) {
  Assert(out + len <= limit, "chunk copy exceeds safety limit");
  if ((limit - out) < (ptrdiff_t)CHUNKCOPY_CHUNK_SIZE) {
    const unsigned char *Z_RESTRICT rfrom = from;
    if (len & 8) {
      Z_BUILTIN_MEMCPY(out, rfrom, 8);
      out += 8;
      rfrom += 8;
    }
    if (len & 4) {
      Z_BUILTIN_MEMCPY(out, rfrom, 4);
      out += 4;
      rfrom += 4;
    }
    if (len & 2) {
      Z_BUILTIN_MEMCPY(out, rfrom, 2);
      out += 2;
      rfrom += 2;
    }
    if (len & 1) {
      *out++ = *rfrom++;
    }
    return out;
  }
  return chunkcopy_core(out, from, len);
}

/**
 * Perform short copies until distance can be rewritten as being at
 * least CHUNKCOPY_CHUNK_SIZE.
 *
 * Assumes it's OK to overwrite at least the first 2*CHUNKCOPY_CHUNK_SIZE
 * bytes of output even if the copy is shorter than this.  This assumption
 * holds within zlib inflate_fast(), which starts every iteration with at
 * least 258 bytes of output space available (258 being the maximum length
 * output from a single token; see inffast.c).
 */
static inline unsigned char *chunkunroll_relaxed(unsigned char *out,
                                                 unsigned *dist,
                                                 unsigned *len) {
  const unsigned char *from = out - *dist;
  while (*dist < *len && *dist < CHUNKCOPY_CHUNK_SIZE) {
    storechunk(out, loadchunk(from));
    out += *dist;
    *len -= *dist;
    *dist += *dist;
  }
  return out;
}

/**
 * v_load64_dup(): load *src as an unaligned 64-bit int and duplicate it
 * in every 64-bit component of the 128-bit result (64-bit int splat).
 */
static inline z_vec128i_t v_load64_dup(const void *src) {
  int64_t i64;
  Z_BUILTIN_MEMCPY(&i64, src, sizeof(i64));
  return _mm_set1_epi64x(i64);
}

/**
 * v_load32_dup(): load *src as an unaligned 32-bit int and duplicate it
 * in every 32-bit component of the 128-bit result (32-bit int splat).
 */
static inline z_vec128i_t v_load32_dup(const void *src) {
  int32_t i32;
  Z_BUILTIN_MEMCPY(&i32, src, sizeof(i32));
  return _mm_set1_epi32(i32);
}

/**
 * v_load16_dup(): load *src as an unaligned 16-bit int and duplicate it
 * in every 16-bit component of the 128-bit result (16-bit int splat).
 */
static inline z_vec128i_t v_load16_dup(const void *src) {
  int16_t i16;
  Z_BUILTIN_MEMCPY(&i16, src, sizeof(i16));
  return _mm_set1_epi16(i16);
}

/**
 * v_load8_dup(): load the 8-bit int *src and duplicate it in every
 * 8-bit component of the 128-bit result (8-bit int splat).
 */
static inline z_vec128i_t v_load8_dup(const void *src) {
  return _mm_set1_epi8(*(const char *)src);
}

/**
 * v_store_128(): store the 128-bit vec in a memory destination (that
 * might not be 16-byte aligned) void* out.
 */
static inline void v_store_128(void *out, const z_vec128i_t vec) {
  _mm_storeu_si128((__m128i *)out, vec);
}

/**
 * Perform an overlapping copy which behaves as a memset() operation,
 * but supporting periods other than one, and assume that length is
 * non-zero and that it's OK to overwrite at least
 * CHUNKCOPY_CHUNK_SIZE*3 bytes of output even if the length is shorter
 * than this.
 */
static inline unsigned char *chunkset_core(unsigned char *out, unsigned period,
                                           unsigned len) {
  z_vec128i_t v;
  const int bump = ((len - 1) % sizeof(v)) + 1;
  switch (period) {
    case 1:
      v = v_load8_dup(out - 1);
      v_store_128(out, v);
      out += bump;
      len -= bump;
      while (len > 0) {
        v_store_128(out, v);
        out += sizeof(v);
        len -= sizeof(v);
      }
      return out;
    case 2:
      v = v_load16_dup(out - 2);
      v_store_128(out, v);
      out += bump;
      len -= bump;
      if (len > 0) {
        v = v_load16_dup(out - 2);
        do {
          v_store_128(out, v);
          out += sizeof(v);
          len -= sizeof(v);
        } while (len > 0);
      }
      return out;
    case 4:
      v = v_load32_dup(out - 4);
      v_store_128(out, v);
      out += bump;
      len -= bump;
      if (len > 0) {
        v = v_load32_dup(out - 4);
        do {
          v_store_128(out, v);
          out += sizeof(v);
          len -= sizeof(v);
        } while (len > 0);
      }
      return out;
    case 8:
      v = v_load64_dup(out - 8);
      v_store_128(out, v);
      out += bump;
      len -= bump;
      if (len > 0) {
        v = v_load64_dup(out - 8);
        do {
          v_store_128(out, v);
          out += sizeof(v);
          len -= sizeof(v);
        } while (len > 0);
      }
      return out;
  }
  out = chunkunroll_relaxed(out, &period, &len);
  return chunkcopy_core(out, out - period, len);
}

/**
 * Perform a memcpy-like operation, but assume that length is non-zero
 * and that it's OK to overwrite at least CHUNKCOPY_CHUNK_SIZE bytes of
 * output even if the length is shorter than this.
 *
 * Unlike chunkcopy_core() above, no guarantee is made regarding the behaviour
 * of overlapping buffers, regardless of the distance between the pointers.
 * This is reflected in the `restrict`-qualified pointers, allowing the
 * compiler to re-order loads and stores.
 */
static inline unsigned char *chunkcopy_relaxed(
    unsigned char *Z_RESTRICT out, const unsigned char *Z_RESTRICT from,
    unsigned len) {
  return chunkcopy_core(out, from, len);
}

/**
 * Like chunkcopy_relaxed(), but avoid writing beyond of legal output.
 *
 * Unlike chunkcopy_core_safe() above, no guarantee is made regarding the
 * behaviour of overlapping buffers, regardless of the distance between the
 * pointers.  This is reflected in the `restrict`-qualified pointers, allowing
 * the compiler to re-order loads and stores.
 *
 * Accepts an additional pointer to the end of safe output.  A generic safe
 * copy would use (out + len), but it's normally the case that the end of the
 * output buffer is beyond the end of the current copy, and this can still be
 * exploited.
 */
static inline unsigned char *chunkcopy_safe(
    unsigned char *out, const unsigned char *Z_RESTRICT from, unsigned len,
    unsigned char *limit) {
  Assert(out + len <= limit, "chunk copy exceeds safety limit");
  return chunkcopy_core_safe(out, from, len, limit);
}

/**
 * Perform chunky copy within the same buffer, where the source and
 * destination may potentially overlap.
 *
 * Assumes that len > 0 on entry, and that it's safe to write at least
 * CHUNKCOPY_CHUNK_SIZE*3 bytes to the output.
 */
static inline unsigned char *chunkcopy_lapped_relaxed(unsigned char *out,
                                                      unsigned dist,
                                                      unsigned len) {
  if (dist < len && dist < CHUNKCOPY_CHUNK_SIZE) {
    return chunkset_core(out, dist, len);
  }
  return chunkcopy_core(out, out - dist, len);
}

/**
 * Behave like chunkcopy_lapped_relaxed(), but avoid writing beyond of
 * legal output.
 *
 * Accepts an additional pointer to the end of safe output.  A generic safe
 * copy would use (out + len), but it's normally the case that the end of the
 * output buffer is beyond the end of the current copy, and this can still be
 * exploited.
 */
static inline unsigned char *chunkcopy_lapped_safe(unsigned char *out,
                                                   unsigned dist, unsigned len,
                                                   unsigned char *limit) {
  Assert(out + len <= limit, "chunk copy exceeds safety limit");
  if ((limit - out) < (ptrdiff_t)(3 * CHUNKCOPY_CHUNK_SIZE)) {
    /* TODO(cavalcantii): try harder to optimise this */
    while (len-- > 0) {
      *out = *(out - dist);
      out++;
    }
    return out;
  }
  return chunkcopy_lapped_relaxed(out, dist, len);
}

#undef Z_STATIC_ASSERT
#undef Z_RESTRICT
#undef Z_BUILTIN_MEMCPY

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* THIRD_PARTY_ZLIB_CHUNKCOPY_H */
