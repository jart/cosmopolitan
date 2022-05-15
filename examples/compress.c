#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/zlib/zlib.h"

#define CHUNK 4096

// clang-format off
// make -j8 o//examples && dd if=/dev/urandom count=100 | tee a | o//examples/compress.com | o//examples/decompress.com >b && sha1sum a b
// clang-format on

int compressor(int infd, int outfd) {
  z_stream zs;
  int rc, flush;
  unsigned have;
  unsigned char *inbuf;
  unsigned char *outbuf;
  inbuf = gc(valloc(CHUNK));
  outbuf = gc(valloc(CHUNK));
  zs.zalloc = 0;
  zs.zfree = 0;
  zs.opaque = 0;
  rc = deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS,
                    DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
  if (rc != Z_OK) return rc;
  do {
    rc = read(infd, inbuf, CHUNK);
    if (rc == -1) {
      deflateEnd(&zs);
      return Z_ERRNO;
    }
    zs.avail_in = rc;
    flush = !rc ? Z_FINISH : Z_SYNC_FLUSH;
    zs.next_in = inbuf;
    do {
      zs.avail_out = CHUNK;
      zs.next_out = outbuf;
      rc = deflate(&zs, flush);
      assert(rc != Z_STREAM_ERROR);
      have = CHUNK - zs.avail_out;
      if (write(outfd, outbuf, have) != have) {
        deflateEnd(&zs);
        return Z_ERRNO;
      }
    } while (!zs.avail_out);
    assert(!zs.avail_in);
  } while (flush != Z_FINISH);
  assert(rc == Z_STREAM_END);
  deflateEnd(&zs);
  return Z_OK;
}

const char *zerr(int rc) {
  switch (rc) {
    case Z_ERRNO:
      return strerror(errno);
    case Z_STREAM_ERROR:
      return "invalid compression level";
    case Z_DATA_ERROR:
      return "invalid or incomplete deflate data";
    case Z_MEM_ERROR:
      return "out of memory";
    case Z_VERSION_ERROR:
      return "zlib version mismatch!";
    default:
      unreachable;
  }
}

int main(int argc, char *argv[]) {
  int rc;
  rc = compressor(0, 1);
  if (rc == Z_OK) {
    return 0;
  } else {
    fprintf(stderr, "error: compressor: %s\n", zerr(rc));
    return 1;
  }
}
