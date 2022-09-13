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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/mem/gc.internal.h"
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
for strategy in F L R H; do
  o/$m/examples/compress.com -$strategy$level <o/dbg/third_party/python/python.com | dd count=10000 2>/tmp/info >/tmp/comp
  compspeed=$(grep -Po '[.\d]+ \w+/s' /tmp/info)
  o/$m/examples/decompress.com </tmp/comp | dd count=10000 2>/tmp/info >/dev/null
  decompspeed=$(grep -Po '[.\d]+ \w+/s' /tmp/info)
  size=$(o/$m/examples/compress.com -$strategy$level <o/$m/examples/compress.com | wc -c)
  echo "level $strategy $level $size compress $compspeed decompress $decompspeed"
done
done
*/
// clang-format on

int decompressor(int infd, int outfd) {
  int rc;
  unsigned have;
  z_stream zs;
  unsigned char *inbuf;
  unsigned char *outbuf;
  inbuf = gc(valloc(CHUNK));
  outbuf = gc(valloc(CHUNK));
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  zs.avail_in = 0;
  zs.next_in = Z_NULL;
  rc = inflateInit(&zs);
  if (rc != Z_OK) return rc;
  do {
    rc = read(infd, inbuf, CHUNK);
    if (rc == -1) {
      inflateEnd(&zs);
      return Z_ERRNO;
    }
    if (!rc) {
      break;
    }
    zs.avail_in = rc;
    zs.next_in = inbuf;
    do {
      zs.avail_out = CHUNK;
      zs.next_out = outbuf;
      rc = inflate(&zs, Z_SYNC_FLUSH);
      assert(rc != Z_STREAM_ERROR);
      switch (rc) {
        case Z_NEED_DICT:
          rc = Z_DATA_ERROR;
          // fallthrough
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
          inflateEnd(&zs);
          return rc;
      }
      have = CHUNK - zs.avail_out;
      if (write(outfd, outbuf, have) != have) {
        inflateEnd(&zs);
        return Z_ERRNO;
      }
    } while (!zs.avail_out);
  } while (rc != Z_STREAM_END);
  inflateEnd(&zs);
  return rc == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
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
  rc = decompressor(0, 1);
  if (rc == Z_OK) {
    return 0;
  } else {
    fprintf(stderr, "error: decompressor: %s\n", zerr(rc));
    return 1;
  }
}
