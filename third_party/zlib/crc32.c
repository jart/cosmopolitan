/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 1995-2006, 2010, 2011, 2012, 2016 Mark Adler                       │
│ Use of this source code is governed by the BSD-style licenses that can       │
│ be found in the third_party/zlib/LICENSE file.                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "third_party/zlib/deflate.h"
#include "third_party/zlib/internal.h"
#include "third_party/zlib/zutil.h"

asm(".ident\t\"\\n\\n\
zlib (zlib License)\\n\
Copyright 1995-2017 Jean-loup Gailly and Mark Adler\"");
asm(".include \"libc/disclaimer.inc\"");

void crc_reset(struct DeflateState *const s) {
  if (X86_HAVE(PCLMUL)) {
    crc_fold_init(s);
    return;
  }
  s->strm->adler = crc32(0L, Z_NULL, 0);
}

void crc_finalize(struct DeflateState *const s) {
  if (X86_HAVE(PCLMUL)) s->strm->adler = crc_fold_512to32(s);
}

void copy_with_crc(z_streamp strm, Bytef *dst, long size) {
  if (X86_HAVE(PCLMUL)) {
    crc_fold_copy(strm->state, dst, strm->next_in, size);
    return;
  }
  memcpy(dst, strm->next_in, size);
  strm->adler = crc32(strm->adler, dst, size);
}
