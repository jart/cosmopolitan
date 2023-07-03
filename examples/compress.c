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
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/zlib/zlib.h"

#define CHUNK 32768

#define USAGE \
  " <STDIN >STDOUT\n\
\n\
SYNOPSIS\n\
\n\
  Zlib Compressor\n\
\n\
FLAGS\n\
\n\
  -?\n\
  -h    help\n\
  -0    disable compression\n\
  -1    fastest compression\n\
  -4    coolest compression\n\
  -9    maximum compression\n\
  -F    fixed strategy (advanced)\n\
  -L    filtered strategy (advanced)\n\
  -R    run length strategy (advanced)\n\
  -H    huffman only strategy (advanced)\n\
\n"

// clang-format off
// make -j8 o//examples && dd if=/dev/urandom count=100 | tee a | o//examples/compress.com | o//examples/decompress.com >b && sha1sum a b
/*
#!/bin/bash
# data file is o/dbg/third_party/python/python.com
# level   1 348739 compress 22.8 MB/s decompress 444 MB/s
# level   2 347549 compress 37.8 MB/s decompress 457 MB/s
# level   3 346902 compress 33.3 MB/s decompress 463 MB/s
# level   4 345671 compress 29.3 MB/s decompress 467 MB/s
# level   5 344392 compress 22.4 MB/s decompress 506 MB/s
# level   6 342105 compress 10.9 MB/s decompress 516 MB/s
# level   7 342046 compress  7.9 MB/s decompress 515 MB/s
# level   8 342009 compress  5.8 MB/s decompress 518 MB/s
# level   9 342001 compress  5.7 MB/s decompress 524 MB/s
# level F 1 362426 compress 48.2 MB/s decompress 488 MB/s
# level F 2 360875 compress 42.7 MB/s decompress 484 MB/s
# level F 3 359992 compress 37.1 MB/s decompress 499 MB/s
# level F 4 358460 compress 32.9 MB/s decompress 503 MB/s
# level F 5 356431 compress 24.0 MB/s decompress 547 MB/s
# level F 6 352274 compress 11.6 MB/s decompress 558 MB/s
# level F 7 352155 compress  8.7 MB/s decompress 554 MB/s
# level F 8 352065 compress  6.3 MB/s decompress 554 MB/s
# level F 9 352051 compress  6.2 MB/s decompress 556 MB/s
# level L 1 348739 compress 41.1 MB/s decompress 446 MB/s
# level L 2 347549 compress 37.4 MB/s decompress 443 MB/s
# level L 3 346902 compress 32.3 MB/s decompress 462 MB/s
# level L 4 351932 compress 28.8 MB/s decompress 511 MB/s
# level L 5 351384 compress 23.6 MB/s decompress 520 MB/s
# level L 6 351328 compress 12.1 MB/s decompress 522 MB/s
# level L 7 351230 compress  7.3 MB/s decompress 518 MB/s
# level L 8 351192 compress  5.7 MB/s decompress 522 MB/s
# level L 9 351182 compress  6.5 MB/s decompress 519 MB/s
# level R 1 388209 compress 83.1 MB/s decompress 371 MB/s
# level R 2 388209 compress 82.3 MB/s decompress 362 MB/s
# level R 3 388209 compress 81.8 MB/s decompress 361 MB/s
# level R 4 388209 compress 81.7 MB/s decompress 364 MB/s
# level R 5 388209 compress 81.7 MB/s decompress 363 MB/s
# level R 6 388209 compress 80.1 MB/s decompress 359 MB/s
# level R 7 388209 compress 80.3 MB/s decompress 354 MB/s
# level R 8 388209 compress 80.3 MB/s decompress 363 MB/s
# level R 9 388209 compress 81.3 MB/s decompress 364 MB/s
# level H 1 390207 compress 87.6 MB/s decompress 371 MB/s
# level H 2 390207 compress 87.5 MB/s decompress 372 MB/s
# level H 3 390207 compress 85.5 MB/s decompress 364 MB/s
# level H 4 390207 compress 87.3 MB/s decompress 375 MB/s
# level H 5 390207 compress 89.0 MB/s decompress 373 MB/s
# level H 6 390207 compress 87.3 MB/s decompress 372 MB/s
# level H 7 390207 compress 87.0 MB/s decompress 368 MB/s
# level H 8 390207 compress 86.2 MB/s decompress 367 MB/s
# level H 9 390207 compress 86.9 MB/s decompress 369 MB/s
m=
make -j8 MODE=$m o/$m/examples || exit
for strategy in ' ' F L R H; do
for level in $(seq 1 9); do
  o/$m/examples/compress.com -$level$strategy <o/dbg/third_party/python/python.com | dd count=10000 2>/tmp/info >/tmp/comp
  compspeed=$(grep -Po '[.\d]+ \w+/s' /tmp/info)
  o/$m/examples/decompress.com </tmp/comp | dd count=10000 2>/tmp/info >/dev/null
  decompspeed=$(grep -Po '[.\d]+ \w+/s' /tmp/info)
  size=$(o/$m/examples/compress.com -$level$strategy <o/$m/examples/compress.com | wc -c)
  echo "level $strategy $level $size compress $compspeed decompress $decompspeed"
done
done
*/
// clang-format on

int level;
int strategy;

wontreturn void PrintUsage(int rc, FILE *f) {
  fputs("usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hFLRH0123456789")) != -1) {
    switch (opt) {
      case 'F':
        strategy = Z_FIXED;
        break;
      case 'L':
        strategy = Z_FILTERED;
        break;
      case 'R':
        strategy = Z_RLE;
        break;
      case 'H':
        strategy = Z_HUFFMAN_ONLY;
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        level = opt - '0';
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

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
  rc = deflateInit2(&zs, level, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, strategy);
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
      __builtin_unreachable();
  }
}

int main(int argc, char *argv[]) {
  int rc;
  level = Z_DEFAULT_COMPRESSION;
  strategy = Z_DEFAULT_STRATEGY;
  GetOpts(argc, argv);
  rc = compressor(0, 1);
  if (rc == Z_OK) {
    return 0;
  } else {
    fprintf(stderr, "error: compressor: %s\n", zerr(rc));
    return 1;
  }
}
