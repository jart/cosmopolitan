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
 * Locates End Of Central Directory record in ZIP file.
 *
 * The ZIP spec says this header can be anywhere in the last 64kb. We
 * search it backwards for the ZIP-64 "PK♠•" magic number. If that's not
 * found, then we search again for the original "PK♣♠" magnum. The
 * caller needs to check the first four bytes of the returned value to
 * determine whether to use ZIP_CDIR_xxx() or ZIP_CDIR64_xxx() macros.
 *
 * @param p points to file memory
 * @param n is byte size of file
 * @return pointer to EOCD64 or EOCD, or NULL if not found
 */
void *GetZipCdir(const uint8_t *p, size_t n) {
  size_t i, j;
  i = n - 4;
  do {
    if (READ32LE(p + i) == kZipCdir64LocatorMagic &&
        i + kZipCdir64LocatorSize <= n &&
        IsZipCdir64(p, n, ZIP_LOCATE64_OFFSET(p + i))) {
      return (void *)(p + ZIP_LOCATE64_OFFSET(p + i));
    } else if (READ32LE(p + i) == kZipCdirHdrMagic && IsZipCdir32(p, n, i)) {
      j = i;
      do {
        if (READ32LE(p + j) == kZipCdir64LocatorMagic &&
            j + kZipCdir64LocatorSize <= n &&
            IsZipCdir64(p, n, ZIP_LOCATE64_OFFSET(p + j))) {
          return (void *)(p + ZIP_LOCATE64_OFFSET(p + j));
        }
      } while (j-- && i - j < 64 * 1024);
      return (void *)(p + i);
    }
  } while (i--);
  return NULL;
}
