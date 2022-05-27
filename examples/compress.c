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
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/zlib/zlib.h"

#define CHUNK 32768

// clang-format off
// make -j8 o//examples && dd if=/dev/urandom count=100 | tee a | o//examples/compress.com | o//examples/decompress.com >b && sha1sum a b
/*
# data file is o/dbg/third_party/python/python.com
# level 0 147517 compress 495 MB/s decompress 1.4 GB/s
# level 1 80274 compress 29.2 MB/s decompress 303 MB/s
# level 2 79384 compress 33.8 MB/s decompress 212 MB/s
# level 3 78875 compress 28.9 MB/s decompress 224 MB/s
# level 4 78010 compress 27.1 MB/s decompress 319 MB/s <-- sweet spot?
# level 5 77107 compress 19.5 MB/s decompress 273 MB/s
# level 6 75081 compress 10.0 MB/s decompress 99.3 MB/s
# level 7 75022 compress 7.5 MB/s decompress 287 MB/s
# level 8 75016 compress 5.4 MB/s decompress 109 MB/s
# level 9 75016 compress 5.4 MB/s decompress 344 MB/s
m=
make -j8 MODE=$m o/$m/examples || exit
for level in $(seq 0 9); do
  o/$m/examples/compress.com $level <o/dbg/third_party/python/python.com | dd count=10000 2>/tmp/info >/tmp/comp
  compspeed=$(grep -Po '[.\d]+ \w+/s' /tmp/info)
  o/$m/examples/decompress.com $level </tmp/comp | dd count=10000 2>/tmp/info >/dev/null
  decompspeed=$(grep -Po '[.\d]+ \w+/s' /tmp/info)
  size=$(o/$m/examples/compress.com $level <o/$m/examples/compress.com | wc -c)
  echo "level $level $size compress $compspeed decompress $decompspeed"
done
*/
// clang-format on

int compressor(int infd, int outfd, int level) {
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
  rc = deflateInit2(&zs, level, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL,
                    Z_DEFAULT_STRATEGY);
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
  int rc, level;
  if (argc > 1) {
    level = atoi(argv[1]);
  } else {
    level = Z_DEFAULT_COMPRESSION;
  }
  rc = compressor(0, 1, level);
  if (rc == Z_OK) {
    return 0;
  } else {
    fprintf(stderr, "error: compressor: %s\n", zerr(rc));
    return 1;
  }
}
