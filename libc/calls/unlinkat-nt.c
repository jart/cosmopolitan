/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/errno.h"

textwindows int sys_unlinkat_nt(int dirfd, const char *path, int flags) {

  // validate flags
  if (flags & ~AT_REMOVEDIR)
    return einval();

  // translate unix to windows path
  int n;
  char16_t path16[PATH_MAX];
  if ((n = __mkntpathat(dirfd, path, path16)) == -1)
    return -1;

  // get info about file (not following symlinks)
  uint32_t dwFileAttrs;
  if ((dwFileAttrs = GetFileAttributes(path16)) == -1u)
    return __fix_enotdir(__winerr(), path16);
  bool bIsActuallyDirectory = (dwFileAttrs & kNtFileAttributeDirectory) &&
                              !(dwFileAttrs & kNtFileAttributeReparsePoint);

  // check for mismatch between intention and file status
  if (flags & AT_REMOVEDIR) {
    if (!bIsActuallyDirectory)
      return enotdir();
  } else {
    if (bIsActuallyDirectory)
      return eisdir();
  }

  // check if containing directory is read only
  while (n && path16[n - 1] != '\\')
    --n;
  while (n && path16[n - 1] == '\\')
    --n;
  char16_t save = path16[n];
  path16[n] = 0;
  uint32_t dwParentAttrs;
  if ((dwParentAttrs = GetFileAttributes(path16)) != -1u)
    if (dwParentAttrs & kNtFileAttributeReadonly)
      return eacces();
  path16[n] = save;

  // unix unlink() is designed to delete read-only files
  // we need to explicitly clear the read-only status on win32
  if (dwFileAttrs & kNtFileAttributeReadonly)
    if (!SetFileAttributes(path16, dwFileAttrs & ~kNtFileAttributeReadonly))
      return eacces();

  // now delete the file
  bool32 ok;
  if (dwFileAttrs & kNtFileAttributeDirectory) {
    ok = RemoveDirectory(path16);
  } else {
    ok = DeleteFile(path16);
  }
  if (!ok) {
    uint32_t dwErrorCode = GetLastError();
    if (dwFileAttrs & kNtFileAttributeReadonly)
      SetFileAttributes(path16, dwFileAttrs);
    errno = __errno_windows2linux(dwErrorCode);
    return __fix_enotdir(-1, path16);
  }
  return 0;
}
