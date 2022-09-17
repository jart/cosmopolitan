#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_
#include "libc/str/str.h"
#include "third_party/zlib/deflate.internal.h"

#define Z_CRC32_SSE42_MINIMUM_LENGTH 64
#define Z_CRC32_SSE42_CHUNKSIZE_MASK 15

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define zmemzero bzero
#define zmemcpy  memmove
#define z_const  const

unsigned deflate_read_buf(z_streamp, Bytef *, unsigned) hidden;
void copy_with_crc(z_streamp, Bytef *, long) hidden;
void crc_finalize(deflate_state *const) hidden;
void crc_reset(deflate_state *const) hidden;
uint32_t adler32_simd_(uint32_t, const unsigned char *, size_t) hidden;
void crc_fold_init(deflate_state *const) hidden;
void crc_fold_copy(deflate_state *const, unsigned char *, const unsigned char *,
                   long) hidden;
unsigned crc_fold_512to32(deflate_state *const) hidden;
void fill_window_sse(deflate_state *) hidden;
void *zcalloc(void *, uInt, uInt) hidden;
void zcfree(void *, void *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_ */
