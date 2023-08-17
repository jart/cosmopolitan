/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.internal.h"

static ssize_t __zipos_scan(struct Zipos *zipos, struct ZiposUri *name) {
  size_t len = name->len;
  if (len && name->path[len - 1] == '/') {
    --len;
  }
  if (!len) {
    return ZIPOS_SYNTHETIC_DIRECTORY;
  }
  bool found_subfile = false;
  size_t c = GetZipCdirOffset(zipos->cdir);
  size_t n = GetZipCdirRecords(zipos->cdir);
  for (size_t i = 0; i < n; ++i, c += ZIP_CFILE_HDRSIZE(zipos->map + c)) {
    const char *zname = ZIP_CFILE_NAME(zipos->map + c);
    size_t zsize = ZIP_CFILE_NAMESIZE(zipos->map + c);
    if ((len == zsize || (len + 1 == zsize && zname[len] == '/')) &&
        !memcmp(name->path, zname, len)) {
      return c;
    } else if (len + 1 < zsize && zname[len] == '/' &&
               !memcmp(name->path, zname, len)) {
      found_subfile = true;
    }
  }
  if (found_subfile) {
    return ZIPOS_SYNTHETIC_DIRECTORY;
  }
  return -1;
}

// support code for open(), stat(), and access()
ssize_t __zipos_find(struct Zipos *zipos, struct ZiposUri *name) {
  ssize_t cf;
  if ((cf = __zipos_scan(zipos, name)) == -1) {
    // test if parent component exists that isn't a directory
    char *p;
    while ((p = memrchr(name->path, '/', name->len))) {
      name->path[name->len = p - name->path] = 0;
      if ((cf = __zipos_scan(zipos, name)) != -1 &&
          cf != ZIPOS_SYNTHETIC_DIRECTORY &&
          !S_ISDIR(GetZipCfileMode(zipos->map + cf))) {
        return enotdir();
      }
    }
    return enoent();
  }
  // test if we're opening "foo/" and "foo" isn't a directory
  if (cf != ZIPOS_SYNTHETIC_DIRECTORY &&  //
      name->len && name->path[name->len - 1] == '/' &&
      !S_ISDIR(GetZipCfileMode(zipos->map + cf))) {
    return enotdir();
  }
  return cf;
}
