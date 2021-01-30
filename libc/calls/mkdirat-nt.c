/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"

static textwindows bool SubpathExistsThatsNotDirectory(char16_t *path) {
  char16_t *p;
  uint32_t attrs;
  while ((p = strrchr16(path, '\\'))) {
    *p = u'\0';
    if ((attrs = GetFileAttributes(path)) != -1u) {
      if (attrs & kNtFileAttributeDirectory) return false;
      return true;
    }
  }
  return false;
}

textwindows int mkdirat$nt(int dirfd, const char *path, uint32_t mode) {
  int e;
  char16_t *p, path16[PATH_MAX];
  if (__mkntpathat(dirfd, path, 0, path16) == -1) return -1;
  if (CreateDirectory(path16, NULL)) return 0;
  e = GetLastError();
  /* WIN32 doesn't distinguish between ENOTDIR and ENOENT */
  if (e == ENOTDIR && !SubpathExistsThatsNotDirectory(path16)) e = ENOENT;
  errno = e;
  return -1;
}
