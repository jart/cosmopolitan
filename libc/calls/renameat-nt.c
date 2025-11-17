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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/limits.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/movefileexflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

textwindows static bool IsDirectory(uint32_t dwFileAttrs) {
  return (dwFileAttrs & kNtFileAttributeDirectory) &&
         !(dwFileAttrs & kNtFileAttributeReparsePoint);
}

textwindows int sys_renameat_nt(int olddirfd, const char *oldpath, int newdirfd,
                                const char *newpath) {

  // allocate memory
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct {
    char16_t oldpath16[PATH_MAX];
    char16_t newpath16[PATH_MAX];
  } M;
  CheckLargeStackAllocation(&M, sizeof(M));
#pragma GCC pop_options

  // translate unix to windows paths
  int n;
  if (((n = __mkntpathat(olddirfd, oldpath, M.oldpath16)) == -1) ||
      __mkntpathat(newdirfd, newpath, M.newpath16) == -1)
    return -1;

  // similar to unlinkat() we need to explicitly check if we have
  // permission to delete an entry from a directory
  if (n && M.oldpath16[n - 1] != '\\') {
    while (n && M.oldpath16[n - 1] != '\\')
      --n;
    while (n && M.oldpath16[n - 1] == '\\')
      --n;
    char16_t save = M.oldpath16[n];
    M.oldpath16[n] = 0;
    uint32_t attr;
    if ((attr = GetFileAttributes(M.oldpath16)) != -1u)
      if (attr & kNtFileAttributeReadonly)
        return eacces();
    M.oldpath16[n] = save;
  }

  // simulate unix directory status errors
  uint32_t oldattr = GetFileAttributes(M.oldpath16);
  uint32_t newattr = GetFileAttributes(M.newpath16);
  if (oldattr != -1u && newattr != -1u) {
    if (!IsDirectory(oldattr) && IsDirectory(newattr))
      return eisdir();
    if (IsDirectory(oldattr) && !IsDirectory(newattr))
      return enotdir();
    if (IsDirectory(oldattr) && IsDirectory(newattr))
      if (!RemoveDirectory(M.newpath16) &&
          GetLastError() == kNtErrorDirNotEmpty)
        return enotempty();
  }

  // rename the file
  if (!MoveFileEx(M.oldpath16, M.newpath16, kNtMovefileReplaceExisting))
    return __fix_enotdir2(-1, M.oldpath16, M.newpath16);

  // we're done
  return 0;
}
