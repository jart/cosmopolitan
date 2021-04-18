/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/zip.h"

/**
 * Returns compressed size in bytes from zip central directory header.
 */
uint64_t GetZipCfileCompressedSize(const uint8_t *z) {
  uint64_t x;
  const uint8_t *p, *pe;
  if ((x = ZIP_CFILE_COMPRESSEDSIZE(z)) == 0xFFFFFFFF) {
    for (p = ZIP_CFILE_EXTRA(z), pe = p + ZIP_CFILE_EXTRASIZE(z); p < pe;
         p += ZIP_EXTRA_SIZE(p)) {
      if (ZIP_EXTRA_HEADERID(p) == kZipExtraZip64 &&
          8 + 8 <= ZIP_EXTRA_CONTENTSIZE(p)) {
        return READ64LE(ZIP_EXTRA_CONTENT(p) + 8);
      }
    }
  }
  return x;
}
