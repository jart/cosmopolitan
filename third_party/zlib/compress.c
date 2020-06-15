/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 1995-2005, 2014, 2016 Jean-loup Gailly, Mark Adler                 │
│ Use of this source code is governed by the BSD-style licenses that can       │
│ be found in the third_party/zlib/LICENSE file.                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/zlib/zlib.h"

asm(".ident\t\"\\n\\n\
zlib (zlib License)\\n\
Copyright 1995-2017 Jean-loup Gailly and Mark Adler\"");
asm(".include \"libc/disclaimer.inc\"");

int compress(Bytef *dest, uLongf *destLen, const Bytef *source,
             uLong sourceLen) {
  return compress2(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
}

/**
 * Compresses the source buffer into the destination buffer. The level
 * parameter has the same meaning as in deflateInit. sourceLen is the
 * byte length of the source buffer. Upon entry, destLen is the total
 * size of the destination buffer, which must be at least 0.1% larger
 * than sourceLen plus 12 bytes. Upon exit, destLen is the actual size
 * of the compressed buffer.
 *
 * @return Z_OK if success, Z_MEM_ERROR if there was not enough memory,
 *     Z_BUF_ERROR if there was not enough room in the output buffer,
 *     Z_STREAM_ERROR if the level parameter is invalid.
 */
int compress2(Bytef *dest, uLongf *destLen, const Bytef *source,
              uLong sourceLen, int level) {
  z_stream stream;
  int err;
  const uInt max = (uInt)-1;
  uLong left;

  left = *destLen;
  *destLen = 0;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  stream.opaque = (voidpf)0;

  err = deflateInit(&stream, level);
  if (err != Z_OK) return err;

  stream.next_out = dest;
  stream.avail_out = 0;
  stream.next_in = (const Bytef *)source;
  stream.avail_in = 0;

  do {
    if (stream.avail_out == 0) {
      stream.avail_out = left > (uLong)max ? max : (uInt)left;
      left -= stream.avail_out;
    }
    if (stream.avail_in == 0) {
      stream.avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
      sourceLen -= stream.avail_in;
    }
    err = deflate(&stream, sourceLen ? Z_NO_FLUSH : Z_FINISH);
  } while (err == Z_OK);

  *destLen = stream.total_out;
  deflateEnd(&stream);
  return err == Z_STREAM_END ? Z_OK : err;
}

/**
 * If the default memLevel or windowBits for deflateInit() is changed,
 * then this function needs to be updated.
 */
uLong compressBound(uLong sourceLen) {
  sourceLen = sourceLen + (sourceLen >> 12) + (sourceLen >> 14) +
              (sourceLen >> 25) + 13;
  /* FIXME(cavalcantii): usage of CRC32 Castagnoli as a hash function
   * for the hash table of symbols used for compression has a side effect
   * where for compression level [4, 5] it will increase the output buffer
   * size by 0.1% (i.e. less than 1%) for a high entropy input (i.e. random
   * data). To avoid a scenario where client code would fail, for safety we
   * increase the expected output size by 0.8% (i.e. 8x more than the worst
   * scenario). See: http://crbug.com/990489
   */
  sourceLen += sourceLen >> 7;  // Equivalent to 1.0078125
  return sourceLen;
}
