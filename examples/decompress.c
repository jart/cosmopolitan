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
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/zlib/zlib.h"

#define CHUNK 32768

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
      __builtin_unreachable();
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
