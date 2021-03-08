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

ssize_t __zipos_find(struct Zipos *zipos, const struct ZiposUri *name) {
  size_t i, cf;
  assert(ZIP_CDIR_MAGIC(zipos->cdir) == kZipCdirHdrMagic);
  for (i = 0, cf = ZIP_CDIR_OFFSET(zipos->cdir);
       i < ZIP_CDIR_RECORDS(zipos->cdir);
       ++i, cf += ZIP_CFILE_HDRSIZE(zipos->map + cf)) {
    assert(ZIP_CFILE_MAGIC(zipos->map + cf) == kZipCfileHdrMagic);
    if (name->len == ZIP_CFILE_NAMESIZE(zipos->map + cf) &&
        memcmp(name->path, ZIP_CFILE_NAME(zipos->map + cf), name->len) == 0) {
      return cf;
    }
  }
  return -1;
}
