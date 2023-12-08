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
#include "libc/zip.internal.h"

/**
 * Determines if ZIP EOCD record seems legit.
 */
int IsZipEocd32(const uint8_t *p, size_t n, size_t i) {
  size_t offset;
  if (i > n || n - i < kZipCdirHdrMinSize) {
    return kZipErrorEocdOffsetOverflow;
  }
  if (ZIP_READ32(p + i) != kZipCdirHdrMagic) {
    return kZipErrorEocdMagicNotFound;
  }
  if (i + ZIP_CDIR_HDRSIZE(p + i) > n) {
    return kZipErrorEocdSizeOverflow;
  }
  if (ZIP_CDIR_DISK(p + i) != ZIP_CDIR_STARTINGDISK(p + i)) {
    return kZipErrorEocdDiskMismatch;
  }
  if (ZIP_CDIR_RECORDSONDISK(p + i) != ZIP_CDIR_RECORDS(p + i)) {
    return kZipErrorEocdRecordsMismatch;
  }
  if (ZIP_CDIR_RECORDS(p + i) * kZipCfileHdrMinSize > ZIP_CDIR_SIZE(p + i)) {
    return kZipErrorEocdRecordsOverflow;
  }
  if (ZIP_CDIR_OFFSET(p + i) == 0xFFFFFFFFu) {
    return kZipErrorEocdRecordsOverflow;
  }
  if (ckd_add(&offset, ZIP_CDIR_OFFSET(p + i), ZIP_CDIR_SIZE(p + i))) {
    return kZipErrorEocdOffsetSizeOverflow;
  }
  if (offset > i) {
    return kZipErrorCdirOffsetPastEocd;
  }
  return kZipOk;
}
