/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.internal.h"

static ssize_t __zipos_match(struct Zipos *z, struct ZiposUri *name, int len,
                             int i) {
  size_t cfile = z->index[i];
  const char *zname = ZIP_CFILE_NAME(z->map + cfile);
  int zsize = ZIP_CFILE_NAMESIZE(z->map + cfile);
  if ((len == zsize || (len + 1 == zsize && zname[len] == '/')) &&
      !memcmp(name->path, zname, len)) {
    return cfile;
  } else if (len + 1 < zsize && zname[len] == '/' &&
             !memcmp(name->path, zname, len)) {
    return ZIPOS_SYNTHETIC_DIRECTORY;
  } else {
    return -1;
  }
}

ssize_t __zipos_scan(struct Zipos *zipos, struct ZiposUri *name) {

  // strip trailing slash from search name
  int len = name->len;
  if (len && name->path[len - 1] == '/') {
    --len;
  }

  // empty string means the /zip root directory
  if (!len) {
    return ZIPOS_SYNTHETIC_DIRECTORY;
  }

  // binary search for leftmost name in central directory
  int l = 0;
  int r = zipos->records;
  while (l < r) {
    int m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    const char *xp = ZIP_CFILE_NAME(zipos->map + zipos->index[m]);
    const char *yp = name->path;
    int xn = ZIP_CFILE_NAMESIZE(zipos->map + zipos->index[m]);
    int yn = len;
    int n = MIN(xn, yn);
    int c;
    if (n) {
      if (!(c = memcmp(xp, yp, n))) {
        c = xn - yn;  // xn and yn are 16-bit
      }
    } else {
      c = xn - yn;
    }
    if (c < 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }

  if (l < zipos->records) {
    int dx;
    size_t cfile = zipos->index[l];
    const char *zname = ZIP_CFILE_NAME(zipos->map + cfile);
    int zsize = ZIP_CFILE_NAMESIZE(zipos->map + cfile);
    if (zsize > len && (dx = '/' - (zname[len] & 255))) {
      // since the index is asciibetical, we need to specially handle
      // the case where, when searching for a directory, regular files
      // exist whose names share the same prefix as the directory name.
      dx = dx > +1 ? +1 : dx;
      dx = dx < -1 ? -1 : dx;
      for (l += dx; 0 <= l && l < zipos->records; l += dx) {
        ssize_t cf;
        if ((cf = __zipos_match(zipos, name, len, l)) != -1) {
          return cf;
        }
        cfile = zipos->index[l];
        zname = ZIP_CFILE_NAME(zipos->map + cfile);
        zsize = ZIP_CFILE_NAMESIZE(zipos->map + cfile);
        if (zsize < len || (len && zname[len - 1] != name->path[len - 1])) {
          break;
        }
      }
    } else {
      // otherwise just return pointer to leftmost record if it matches
      return __zipos_match(zipos, name, len, l);
    }
  }

  // otherwise return not found
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
