/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/serialize.h"
#include "libc/intrin/bswap.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "net/http/http.h"

static const char kNoCompressExts[][8] = {
    "bz2",   //
    "gif",   //
    "gz",    //
    "jpg",   //
    "lz4",   //
    "mp4",   //
    "mpeg",  //
    "mpg",   //
    "png",   //
    "webp",  //
    "xz",    //
    "zip",   //
};

static bool BisectNoCompressExts(uint64_t ext) {
  int m, l, r;
  l = 0;
  r = ARRAYLEN(kNoCompressExts) - 1;
  while (l <= r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (READ64BE(kNoCompressExts[m]) < ext) {
      l = m + 1;
    } else if (READ64BE(kNoCompressExts[m]) > ext) {
      r = m - 1;
    } else {
      return true;
    }
  }
  return false;
}

bool IsNoCompressExt(const char *p, size_t n) {
  int c, i;
  uint64_t w;
  if (n == -1) n = p ? strlen(p) : 0;
  if (n) {
    for (i = w = 0; n--;) {
      c = p[n] & 255;
      if (c == '.') break;
      if (++i > 8) return false;
      w <<= 8;
      w |= kToLower[c];
    }
    return BisectNoCompressExts(bswap_64(w));
  }
  return false;
}
