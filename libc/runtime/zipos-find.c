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
#include "libc/runtime/zipos.internal.h"
#include "libc/zip.internal.h"

ssize_t __zipos_find(struct Zipos *zipos, struct ZiposUri *name) {
  if (!name->len) {
    return ZIPOS_SYNTHETIC_DIRECTORY;
  }
  bool found_subfile = false;
  size_t c = GetZipCdirOffset(zipos->cdir);
  size_t n = GetZipCdirRecords(zipos->cdir);
  for (size_t i = 0; i < n; ++i, c += ZIP_CFILE_HDRSIZE(zipos->map + c)) {
    const char *zname = ZIP_CFILE_NAME(zipos->map + c);
    size_t zsize = ZIP_CFILE_NAMESIZE(zipos->map + c);
    if ((name->len == zsize ||
         (name->len + 1 == zsize && zname[name->len] == '/')) &&
        !memcmp(name->path, zname, name->len)) {
      return c;
    } else if (name->len + 1 < zsize && zname[name->len] == '/' &&
               !memcmp(name->path, zname, name->len)) {
      found_subfile = true;
    }
  }
  if (found_subfile) {
    return ZIPOS_SYNTHETIC_DIRECTORY;
  }
  return -1;
}
