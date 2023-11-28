#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_
#include "libc/str/str.h"
#include "third_party/zlib/deflate.internal.h"
#include "third_party/zlib/zconf.h"

#define Z_CRC32_SSE42_MINIMUM_LENGTH 64
#define Z_CRC32_SSE42_CHUNKSIZE_MASK 15

COSMOPOLITAN_C_START_

#define zmemzero bzero
#define zmemcpy  memmove

unsigned deflate_read_buf(z_streamp, Bytef *, unsigned);
void copy_with_crc(z_streamp, Bytef *, long);
void crc_finalize(deflate_state *const);
void crc_reset(deflate_state *const);
uint32_t adler32_simd_(uint32_t, const unsigned char *, size_t);
void crc_fold_init(deflate_state *const);
void crc_fold_copy(deflate_state *const, unsigned char *, const unsigned char *,
                   long);
unsigned crc_fold_512to32(deflate_state *const);
void fill_window_sse(deflate_state *);
void *zcalloc(void *, uInt, uInt);
void zcfree(void *, void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_ */
