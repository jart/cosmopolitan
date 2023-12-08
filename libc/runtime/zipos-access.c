/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.internal.h"

// TODO: this should check parent directory components

/**
 * Checks access metadata in αcτµαlly pδrταblε εxεcµταblε object store.
 *
 * @param uri is obtained via __zipos_parseuri()
 * @asyncsignalsafe
 */
int __zipos_access(struct ZiposUri *name, int amode) {

  struct Zipos *z;
  if (!(z = __zipos_get())) {
    return enoexec();
  }

  ssize_t cf;
  if ((cf = __zipos_find(z, name)) == -1) {
    return -1;
  }

  int mode;
  if (cf != ZIPOS_SYNTHETIC_DIRECTORY) {
    mode = GetZipCfileMode(z->map + cf);
  } else {
    mode = S_IFDIR | 0555;
  }
  if (amode == F_OK) {
    return 0;
  }
  if (amode & ~(R_OK | W_OK | X_OK)) {
    return einval();
  }
  if (((amode & X_OK) && !(mode & 0111)) ||
      ((amode & W_OK) && !(mode & 0222)) ||
      ((amode & R_OK) && !(mode & 0444))) {
    return eacces();
  }

  return 0;
}
