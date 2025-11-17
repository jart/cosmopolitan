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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

textwindows static ssize_t sys_readlinkat_nt_impl(int dirfd, const char *path,
                                                  char *buf, size_t bufsiz,
                                                  char16_t path16[PATH_MAX],
                                                  char path8[PATH_MAX]) {

  // convert link path to win32 style
  if (__mkntpathat(dirfd, path, path16) == -1)
    return -1;

  // check this is a symbolic link, however we want to punt the
  // existence check to CreateFile() which can spot loop errors
  uint32_t dwFileAttrs;
  if ((dwFileAttrs = GetFileAttributes(path16)) != -1u)
    if (!(dwFileAttrs & kNtFileAttributeReparsePoint))
      return einval();

  // open file
  int64_t h;
  if ((h = CreateFile(path16, kNtFileReadAttributes,
                      kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
                      0, kNtOpenExisting,
                      kNtFileFlagOpenReparsePoint | kNtFileFlagBackupSemantics,
                      0)) == -1)
    return __fix_enotdir(__winerr(), path16);

  // read symlink as utf-8 unix path
  // otherwise enomem or eacces could happen here
  int path8len;
  if ((path8len = __readntsym(h, path8, 0)) == -1) {
    CloseHandle(h);
    return -1;
  }

  // copy result without nul termination
  CloseHandle(h);
  int copied = MIN(path8len, bufsiz);
  if (copied)
    memcpy(buf, path8, copied);
  return copied;
}

textwindows ssize_t sys_readlinkat_nt(int dirfd, const char *path, char *buf,
                                      size_t bufsiz) {
  if (bufsiz && kisdangerous(buf))
    return efault();
  ssize_t rc;
  union {
    char path8[PATH_MAX];
    char16_t path16[PATH_MAX];
  } u;
  BLOCK_SIGNALS;
  rc = sys_readlinkat_nt_impl(dirfd, path, buf, bufsiz, u.path16, u.path8);
  ALLOW_SIGNALS;
  return rc;
}
