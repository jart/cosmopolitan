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
#include "libc/stdckdint.h"
#include "libc/zip.h"

/**
 * Returns kZipOk if zip64 end of central directory header seems legit.
 */
int IsZipEocd64(const uint8_t *p, size_t n, size_t i) {
  size_t off, loc;
  uint64_t cdsize;
  if (i > n || i + kZipCdir64HdrMinSize > n) {
    return kZipErrorEocdOffsetOverflow;
  }
  if (ZIP_READ32(p + i) != kZipCdir64HdrMagic) {
    return kZipErrorEocdMagicNotFound;
  }
  if (ckd_add(&loc, i, ZIP_CDIR64_HDRSIZE(p + i)) ||  //
      ckd_add(&off, loc, kZipCdir64LocatorSize) ||    //
      off > n) {                                      //
    return kZipErrorEocdSizeOverflow;
  }
  if (ZIP_LOCATE64_MAGIC(p + loc) != kZipCdir64LocatorMagic) {
    return kZipErrorEocdLocatorMagic;
  }
  if (ZIP_LOCATE64_OFFSET(p + loc) != i) {
    return kZipErrorEocdLocatorOffset;
  }
  if (ckd_add(&off, ZIP_CDIR64_OFFSET(p + i), ZIP_CDIR64_SIZE(p + i))) {
    return kZipErrorEocdOffsetSizeOverflow;
  }
  if (off > i) {
    return kZipErrorCdirOffsetPastEocd;
  }
  if (ckd_mul(&cdsize, ZIP_CDIR64_RECORDS(p + i), kZipCfileHdrMinSize) ||
      cdsize > ZIP_CDIR64_SIZE(p + i)) {
    return kZipErrorEocdRecordsOverflow;
  }
  return kZipOk;
}
