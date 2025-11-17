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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/ctype.h"
#include "libc/errno.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/str/str.h"

textwindows bool __hasregularparent(char16_t *p) {
  bool first = true;
  char16_t *e = p + strlen16(p);
  while (e > p) {
    if (e[-1] == '\\')
      break;
    if (e - p >= 2 && e[-1] == '?' && e[-2] == '\\')
      break;
    if (e - p >= 3 && e[-1] == '?' && e[-2] == '?' && e[-3] == '\\')
      break;
    if (e - p >= 3 && e[-1] == ':' && isalpha(e[-2]) && e[-3] == '\\')
      break;
    if (first) {
      first = false;
    } else {
      uint32_t dwFileAttrs;
      if ((dwFileAttrs = GetFileAttributes(p)) != -1u)
        return !(dwFileAttrs & kNtFileAttributeDirectory);
    }
    while (e > p) {
      int c = e[-1];
      *--e = 0;
      if (c == '\\')
        break;
    }
  }
  return false;
}

// When files don't exist, normally UNIX systems raise ENOENT, except if
// one of the parent directory components in the path exists, but is not
// actually a dirctory, in which case ENOTDIR is raised instead. Example
// would be stat("/etc/passwd/lol") raising ENOTDIR since /etc/passwd is
// obviously a regular file.
textwindows int64_t __fix_enotdir(int64_t rc, char16_t *path) {
  if (rc == -1) {
    switch (errno) {
      case ENOTDIR:
        if (!__hasregularparent(path))
          errno = ENOENT;
        break;
      case ENOENT:
        if (__hasregularparent(path))
          errno = ENOTDIR;
        break;
      default:
        break;
    }
  }
  return rc;
}
