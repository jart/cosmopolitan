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
#include "libc/fmt/wintime.internal.h"
#include "libc/zip.internal.h"

static inline int pop(int x) {
  return !!(x & 1) + !!(x & 2) + !!(x & 4);
}

/**
 * Extracts modified/access/creation timestamps from zip entry.
 *
 * @param cf is pointer to central directory header for file
 * @param mtim optionally receives last modified timestamp
 * @param atim optionally receives modified timestamp
 * @param ctim optionally receives creation timestamp
 * @param gmtoff is seconds adjustment for legacy dos timestamps
 */
void GetZipCfileTimestamps(const uint8_t *cf, struct timespec *mtim,
                           struct timespec *atim, struct timespec *ctim,
                           int gmtoff) {
  const uint8_t *p, *pe;
  if (mtim) *mtim = (struct timespec){0};
  if (atim) *atim = (struct timespec){0};
  if (ctim) *ctim = (struct timespec){0};
  for (p = ZIP_CFILE_EXTRA(cf), pe = p + ZIP_CFILE_EXTRASIZE(cf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraNtfs &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 &&
        ZIP_READ16(ZIP_EXTRA_CONTENT(p) + 4) == 1 &&
        ZIP_READ16(ZIP_EXTRA_CONTENT(p) + 6) >= 8) {
      if (mtim) {
        *mtim = WindowsTimeToTimeSpec(ZIP_READ64(ZIP_EXTRA_CONTENT(p) + 8));
      }
      if (atim && ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 * 2 &&
          ZIP_READ16(ZIP_EXTRA_CONTENT(p) + 6) >= 16) {
        *atim = WindowsTimeToTimeSpec(ZIP_READ64(ZIP_EXTRA_CONTENT(p) + 8 * 2));
      }
      if (ctim && ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 * 3 &&
          ZIP_READ16(ZIP_EXTRA_CONTENT(p) + 6) >= 24) {
        *ctim = WindowsTimeToTimeSpec(ZIP_READ64(ZIP_EXTRA_CONTENT(p) + 8 * 3));
      }
      return;
    }
  }
  for (p = ZIP_CFILE_EXTRA(cf), pe = p + ZIP_CFILE_EXTRASIZE(cf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraExtendedTimestamp &&
        ZIP_EXTRA_CONTENTSIZE(p) > 1 &&
        ZIP_EXTRA_CONTENTSIZE(p) == 1 + 4 * pop(*ZIP_EXTRA_CONTENT(p) & 7)) {
      if (mtim) {
        if (*ZIP_EXTRA_CONTENT(p) & 1) {
          mtim->tv_sec = (int32_t)ZIP_READ32(ZIP_EXTRA_CONTENT(p) + 1);
        } else {
          mtim->tv_sec = DosDateTimeToUnix(ZIP_CFILE_LASTMODIFIEDDATE(cf),
                                           ZIP_CFILE_LASTMODIFIEDTIME(cf)) -
                         gmtoff;
        }
      }
      if (atim && (*ZIP_EXTRA_CONTENT(p) & 2)) {
        atim->tv_sec = (int32_t)ZIP_READ32(ZIP_EXTRA_CONTENT(p) + 1 +
                                           4 * (*ZIP_EXTRA_CONTENT(p) & 1));
      }
      if (ctim && (*ZIP_EXTRA_CONTENT(p) & 4)) {
        ctim->tv_sec = (int32_t)ZIP_READ32(ZIP_EXTRA_CONTENT(p) + 1 +
                                           4 * pop(*ZIP_EXTRA_CONTENT(p) & 3));
      }
      return;
    }
  }
  for (p = ZIP_CFILE_EXTRA(cf), pe = p + ZIP_CFILE_EXTRASIZE(cf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraUnix &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4) {
      if (atim) atim->tv_sec = (int32_t)ZIP_READ32(ZIP_EXTRA_CONTENT(p) + 0);
      if (mtim) mtim->tv_sec = (int32_t)ZIP_READ32(ZIP_EXTRA_CONTENT(p) + 4);
      return;
    }
  }
  if (mtim) {
    mtim->tv_sec = DosDateTimeToUnix(ZIP_CFILE_LASTMODIFIEDDATE(cf),
                                     ZIP_CFILE_LASTMODIFIEDTIME(cf)) -
                   gmtoff;
  }
}
