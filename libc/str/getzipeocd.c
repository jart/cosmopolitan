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

typedef char v16qi __attribute__((__vector_size__(16)));
typedef short v8hi __attribute__((__vector_size__(16)));
typedef long long v2di __attribute__((__vector_size__(16), __aligned__(1)));

/**
 * Locates End Of Central Directory record in ZIP file.
 *
 * The ZIP spec says this header can be anywhere in the last 64kb. We
 * search it backwards for the ZIP-64 "PK♠•" magic number. If that's not
 * found, then we search again for the original "PK♣♠" magnum. The
 * caller needs to check the first four bytes of the returned value to
 * determine whether to use ZIP_CDIR_xxx() or ZIP_CDIR64_xxx() macros.
 *
 * @param f points to file memory
 * @param n is byte size of file
 * @param e may receive error code when null is returned
 * @return pointer to EOCD64 or EOCD, otherwise null
 */
void *GetZipEocd(const void *f, size_t n, int *e) {
  int err;
  size_t i, j;
  uint32_t magic;
  const uint8_t *p;
  p = f;
  i = n - 4;
  err = kZipErrorEocdNotFound;
  do {
#if defined(__x86_64__) && defined(__GNUC__) && !defined(__llvm__) && \
    !defined(__chibicc__)
    v8hi pk = {ZIP_READ16("PK"), ZIP_READ16("PK"), ZIP_READ16("PK"),
               ZIP_READ16("PK"), ZIP_READ16("PK"), ZIP_READ16("PK"),
               ZIP_READ16("PK"), ZIP_READ16("PK")};
    asm("" : "+x"(pk));
    if (i >= 14) {
      v2di x = *(const v2di *)(p + i - 14);
      if (!(__builtin_ia32_pmovmskb128(
                (v16qi)__builtin_ia32_pcmpeqw128((v8hi)x, pk)) |
            __builtin_ia32_pmovmskb128((v16qi)__builtin_ia32_pcmpeqw128(
                (v8hi)__builtin_ia32_psrldqi128(x, 8), pk)))) {
        i -= 13;
        continue;
      }
    }
#endif
    while (magic = ZIP_READ32(p + i), magic != kZipCdir64LocatorMagic &&
                                          magic != kZipCdirHdrMagic &&
                                          i + 0x10000 + 0x1000 >= n && i > 0) {
      --i;
    }
    if (magic == kZipCdir64LocatorMagic && i + kZipCdir64LocatorSize <= n &&
        (err = IsZipEocd64(p, n, ZIP_LOCATE64_OFFSET(p + i))) == kZipOk) {
      return (void *)(p + ZIP_LOCATE64_OFFSET(p + i));
    } else if (magic == kZipCdirHdrMagic &&
               (err = IsZipEocd32(p, n, i)) == kZipOk) {
      j = i;
      do {
        if (ZIP_READ32(p + j) == kZipCdir64LocatorMagic &&
            j + kZipCdir64LocatorSize <= n &&
            IsZipEocd64(p, n, ZIP_LOCATE64_OFFSET(p + j)) == kZipOk) {
          return (void *)(p + ZIP_LOCATE64_OFFSET(p + j));
        }
      } while (j-- && i - j < 128);
      return (void *)(p + i);
    }
  } while (i > 0 && i-- + 0x10000 + 0x1000 >= n);
  if (e) *e = err;
  return 0;
}
