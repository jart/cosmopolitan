/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

static int popcnt3(int x) {
  return !!(x & 1) + !!(x & 2) + !!(x & 4);
}

static struct timespec WindowsTimeToTime(uint64_t x) {
  return (struct timespec){x / HECTONANOSECONDS - MODERNITYSECONDS,
                           x % HECTONANOSECONDS * 100};
}

static void GetZipCfileTimestamps(const uint8_t *zcf, struct stat *st) {
  const uint8_t *p, *pe;
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraNtfs &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 4) == 1 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 6) >= 8) {
      st->st_mtim = WindowsTimeToTime(READ64LE(ZIP_EXTRA_CONTENT(p) + 8));
      if (ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 * 2 &&
          READ16LE(ZIP_EXTRA_CONTENT(p) + 6) >= 16) {
        st->st_atim = WindowsTimeToTime(READ64LE(ZIP_EXTRA_CONTENT(p) + 8 * 2));
      }
      if (ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 * 3 &&
          READ16LE(ZIP_EXTRA_CONTENT(p) + 6) >= 24) {
        st->st_ctim = WindowsTimeToTime(READ64LE(ZIP_EXTRA_CONTENT(p) + 8 * 3));
      }
      return;
    }
  }
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraExtendedTimestamp &&
        ZIP_EXTRA_CONTENTSIZE(p) > 1 &&
        ZIP_EXTRA_CONTENTSIZE(p) ==
            1 + 4 * popcnt3(*ZIP_EXTRA_CONTENT(p) & 7)) {
      if (*ZIP_EXTRA_CONTENT(p) & 1) {
        st->st_mtim.tv_sec = READ32LE(ZIP_EXTRA_CONTENT(p) + 1);
      }
      if (*ZIP_EXTRA_CONTENT(p) & 2) {
        st->st_atim.tv_sec = READ32LE(ZIP_EXTRA_CONTENT(p) + 1 +
                                      4 * (*ZIP_EXTRA_CONTENT(p) & 1));
      }
      if (*ZIP_EXTRA_CONTENT(p) & 4) {
        st->st_ctim.tv_sec = READ32LE(ZIP_EXTRA_CONTENT(p) + 1 +
                                      4 * popcnt3(*ZIP_EXTRA_CONTENT(p) & 3));
      }
      return;
    }
  }
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraUnix &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4) {
      st->st_atim.tv_sec = (int32_t)READ32LE(ZIP_EXTRA_CONTENT(p) + 0);
      st->st_mtim.tv_sec = (int32_t)READ32LE(ZIP_EXTRA_CONTENT(p) + 4);
      return;
    }
  }
  st->st_mtim.tv_sec = DosDateTimeToUnix(ZIP_CFILE_LASTMODIFIEDDATE(zcf),
                                         ZIP_CFILE_LASTMODIFIEDTIME(zcf));
}

int __zipos_stat_impl(struct Zipos *zipos, size_t cf, struct stat *st) {
  size_t lf;
  if (zipos && st) {
    memset(st, 0, sizeof(*st));
    if (ZIP_CFILE_FILEATTRCOMPAT(zipos->map + cf) == kZipOsUnix) {
      st->st_mode = ZIP_CFILE_EXTERNALATTRIBUTES(zipos->map + cf) >> 16;
    } else {
      st->st_mode = 0100644;
    }
    lf = GetZipCfileOffset(zipos->map + cf);
    st->st_size = GetZipLfileUncompressedSize(zipos->map + lf);
    st->st_blocks =
        roundup(GetZipLfileCompressedSize(zipos->map + lf), 512) / 512;
    GetZipCfileTimestamps(zipos->map + cf, st);
    return 0;
  } else {
    return einval();
  }
}
