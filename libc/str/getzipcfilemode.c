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
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/sysv/consts/s.h"
#include "libc/zip.internal.h"

static int ConvertWindowsToUnixMode(int x) {
  int m;
  if (x & kNtFileAttributeReadonly) {
    m = 0444;
  } else {
    m = 0644;
  }
  if (x & kNtFileAttributeDirectory) {
    m |= S_IFDIR | 0111;
  } else {
    m |= S_IFREG;
  }
  return m;
}

/**
 * Returns st_mode from ZIP central directory record.
 */
int GetZipCfileMode(const uint8_t *p) {
  if (ZIP_CFILE_FILEATTRCOMPAT(p) == kZipOsUnix) {
    return ZIP_CFILE_EXTERNALATTRIBUTES(p) >> 16;
  } else {
    return ConvertWindowsToUnixMode(ZIP_CFILE_EXTERNALATTRIBUTES(p));
  }
}
