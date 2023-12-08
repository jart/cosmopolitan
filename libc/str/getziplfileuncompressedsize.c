/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/zip.internal.h"

/**
 * Returns uncompressed size in bytes from zip local file header.
 */
int64_t GetZipLfileUncompressedSize(const uint8_t *z) {
  if (ZIP_LFILE_UNCOMPRESSEDSIZE(z) != 0xFFFFFFFFu) {
    return ZIP_LFILE_UNCOMPRESSEDSIZE(z);
  }
  const uint8_t *p = ZIP_LFILE_EXTRA(z);
  const uint8_t *pe = p + ZIP_LFILE_EXTRASIZE(z);
  for (; p + ZIP_EXTRA_SIZE(p) <= pe; p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraZip64) {
      int offset = 0;
      if (ZIP_LFILE_COMPRESSEDSIZE(z) == 0xFFFFFFFFu) {
        offset += 8;
      }
      if (offset + 8 <= ZIP_EXTRA_CONTENTSIZE(p)) {
        return ZIP_READ64(ZIP_EXTRA_CONTENT(p) + offset);
      }
    }
  }
  return -1;
}
