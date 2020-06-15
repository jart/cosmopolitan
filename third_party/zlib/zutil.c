/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 1995-2017 Jean-loup Gailly                                         │
│ Use of this source code is governed by the BSD-style licenses that can       │
│ be found in the third_party/zlib/LICENSE file.                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/mem/mem.h"
#include "third_party/zlib/zutil.h"

asm(".ident\t\"\\n\\n\
zlib (zlib License)\\n\
Copyright 1995-2017 Jean-loup Gailly and Mark Adler\"");
asm(".include \"libc/disclaimer.inc\"");

const char *const z_errmsg[10] = {
    (const char *)"need dictionary",     /* Z_NEED_DICT       2  */
    (const char *)"stream end",          /* Z_STREAM_END      1  */
    (const char *)"",                    /* Z_OK              0  */
    (const char *)"file error",          /* Z_ERRNO         (-1) */
    (const char *)"stream error",        /* Z_STREAM_ERROR  (-2) */
    (const char *)"data error",          /* Z_DATA_ERROR    (-3) */
    (const char *)"insufficient memory", /* Z_MEM_ERROR     (-4) */
    (const char *)"buffer error",        /* Z_BUF_ERROR     (-5) */
    (const char *)"",                    /* Z_VERSION_ERROR (-6) */
    (const char *)"",
};

const char *zlibVersion() { return ZLIB_VERSION; }

uLong zlibCompileFlags() {
  uLong flags;
  flags = 0;
  switch ((int)(sizeof(uInt))) {
    case 2:
      break;
    case 4:
      flags += 1;
      break;
    case 8:
      flags += 2;
      break;
    default:
      flags += 3;
  }
  switch ((int)(sizeof(uLong))) {
    case 2:
      break;
    case 4:
      flags += 1 << 2;
      break;
    case 8:
      flags += 2 << 2;
      break;
    default:
      flags += 3 << 2;
  }
  switch ((int)(sizeof(voidpf))) {
    case 2:
      break;
    case 4:
      flags += 1 << 4;
      break;
    case 8:
      flags += 2 << 4;
      break;
    default:
      flags += 3 << 4;
  }
  switch ((int)(sizeof(int64_t))) {
    case 2:
      break;
    case 4:
      flags += 1 << 6;
      break;
    case 8:
      flags += 2 << 6;
      break;
    default:
      flags += 3 << 6;
  }
#ifdef ZLIB_DEBUG
  flags += 1 << 8;
#endif
#if defined(ASMV) || defined(ASMINF)
  flags += 1 << 9;
#endif
#ifdef ZLIB_WINAPI
  flags += 1 << 10;
#endif
#ifdef BUILDFIXED
  flags += 1 << 12;
#endif
#ifdef DYNAMIC_CRC_TABLE
  flags += 1 << 13;
#endif
#ifdef NO_GZCOMPRESS
  flags += 1L << 16;
#endif
#ifdef NO_GZIP
  flags += 1L << 17;
#endif
#ifdef PKZIP_BUG_WORKAROUND
  flags += 1L << 20;
#endif
#ifdef FASTEST
  flags += 1L << 21;
#endif
  return flags;
}

#ifdef ZLIB_DEBUG
#ifndef verbose
#define verbose 0
#endif
int z_verbose hidden = verbose;

void z_error(char *m) {
  fprintf(stderr, "%s\n", m);
  exit(1);
}
#endif

/**
 * Exported to allow conversion of error code to string for compress()
 * and uncompress()
 */
const char *zError(int err) { return ERR_MSG(err); }
