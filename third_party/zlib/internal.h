#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_
#include "libc/str/str.h"
#include "third_party/zlib/deflate.internal.h"
#include "third_party/zlib/zconf.h"

#define Z_CRC32_SSE42_MINIMUM_LENGTH 64
#define Z_CRC32_SSE42_CHUNKSIZE_MASK 15

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define zmemzero bzero
#define zmemcpy  memmove

unsigned deflate_read_buf(z_streamp, Bytef *, unsigned) _Hide;
void copy_with_crc(z_streamp, Bytef *, long) _Hide;
void crc_finalize(deflate_state *const) _Hide;
void crc_reset(deflate_state *const) _Hide;
uint32_t adler32_simd_(uint32_t, const unsigned char *, size_t) _Hide;
void crc_fold_init(deflate_state *const) _Hide;
void crc_fold_copy(deflate_state *const, unsigned char *, const unsigned char *,
                   long) _Hide;
unsigned crc_fold_512to32(deflate_state *const) _Hide;
void fill_window_sse(deflate_state *) _Hide;
void *zcalloc(void *, uInt, uInt) _Hide;
void zcfree(void *, void *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_INTERNAL_H_ */
