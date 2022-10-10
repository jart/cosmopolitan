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
#include "ape/relocations.h"
#include "libc/assert.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

// TODO(jart): improve time complexity here

ssize_t __zipos_find(struct Zipos *zipos, const struct ZiposUri *name) {
  const char *zname;
  size_t i, n, c, znamesize;
  if (!name->len) {
    return 0;
  }
  c = GetZipCdirOffset(zipos->cdir);
  n = GetZipCdirRecords(zipos->cdir);
  for (i = 0; i < n; ++i, c += ZIP_CFILE_HDRSIZE(zipos->map + c)) {
    _npassert(ZIP_CFILE_MAGIC(zipos->map + c) == kZipCfileHdrMagic);
    zname = ZIP_CFILE_NAME(zipos->map + c);
    znamesize = ZIP_CFILE_NAMESIZE(zipos->map + c);
    if ((name->len == znamesize && !memcmp(name->path, zname, name->len)) ||
        (name->len + 1 == znamesize && !memcmp(name->path, zname, name->len) &&
         zname[name->len] == '/')) {
      return c;
    } else if ((name->len < znamesize &&
                !memcmp(name->path, zname, name->len) &&
                zname[name->len - 1] == '/') ||
               (name->len + 1 < znamesize &&
                !memcmp(name->path, zname, name->len) &&
                zname[name->len] == '/')) {
      return 0;
    }
  }
  return -1;
}
