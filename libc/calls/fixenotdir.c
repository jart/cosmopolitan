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
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/str/str.h"

static textwindows bool SubpathExistsThatsNotDirectory(char16_t *path) {
  char16_t *p;
  uint32_t attrs;
  while ((p = strrchr16(path, '\\'))) {
    *p = u'\0';
    if ((attrs = GetFileAttributes(path)) != -1u &&
        !(attrs & kNtFileAttributeDirectory)) {
      return true;
    }
  }
  return false;
}

textwindows dontinline int64_t __fix_enotdir3(int64_t rc, char16_t *path1,
                                              char16_t *path2) {
  if (rc == -1 && (errno == kNtErrorPathNotFound ||  // Windows returns ENOTDIR
                   errno == kNtErrorInvalidName)) {  // e.g. has trailing slash
    bool isdir = false;
    if ((!path1 || !(isdir |= SubpathExistsThatsNotDirectory(path1))) &&
        (!path2 || !(isdir |= SubpathExistsThatsNotDirectory(path2)))) {
      errno = kNtErrorFileNotFound;  // ENOENT
    } else if (isdir) {
      errno = kNtErrorPathNotFound;  // ENOTDIR
    }
  }
  return rc;
}

// WIN32 doesn't distinguish between ENOTDIR and ENOENT. UNIX strictly
// requires that a directory component *exists* but is not a directory
// whereas WIN32 will return ENOTDIR if a dir label simply isn't found
//
// - ENOTDIR: A component used as a directory in pathname is not, in
//   fact, a directory. -or- pathname is relative and dirfd is a file
//   descriptor referring to a file other than a directory.
//
// - ENOENT: A directory component in pathname does not exist or is a
//   dangling symbolic link.
//
textwindows int64_t __fix_enotdir(int64_t rc, char16_t *path) {
  return __fix_enotdir3(rc, path, 0);
}
