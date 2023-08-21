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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/movefileexflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

__msabi extern typeof(GetFileAttributes) *const __imp_GetFileAttributesW;
__msabi extern typeof(RemoveDirectory) *const __imp_RemoveDirectoryW;

static textwindows bool StripTrailingSlash(char16_t *path) {
  size_t n = strlen16(path);
  bool had_trailing_slash = false;
  if (n > 1 && path[n - 1] == '\\') {
    had_trailing_slash = true;
    path[--n] = 0;
  }
  return had_trailing_slash;
}

textwindows int sys_renameat_nt(int olddirfd, const char *oldpath, int newdirfd,
                                const char *newpath) {

  // translate unix to windows paths
  char16_t oldpath16[PATH_MAX];
  char16_t newpath16[PATH_MAX];
  if (__mkntpathat(olddirfd, oldpath, 0, oldpath16) == -1 ||
      __mkntpathat(newdirfd, newpath, 0, newpath16) == -1) {
    return -1;
  }

  // strip trailing slash
  // win32 will einval otherwise
  // normally this is handled by __fix_enotdir()
  bool old_must_be_dir = StripTrailingSlash(oldpath16);
  bool new_must_be_dir = StripTrailingSlash(newpath16);

  // test for some known error conditions ahead of time
  // the enotdir check can't be done reactively
  // ideally we should resolve symlinks first
  uint32_t oldattr = __imp_GetFileAttributesW(oldpath16);
  uint32_t newattr = __imp_GetFileAttributesW(newpath16);
  if ((old_must_be_dir && oldattr != -1u &&
       !(oldattr & kNtFileAttributeDirectory)) ||
      (new_must_be_dir && newattr != -1u &&
       !(newattr & kNtFileAttributeDirectory))) {
    return enotdir();
  }
  if (oldattr != -1u && newattr != -1u) {
    if (!(oldattr & kNtFileAttributeDirectory) &&
        (newattr & kNtFileAttributeDirectory)) {
      return eisdir();  // new is directory, but old isn't a directory
    } else if ((oldattr & kNtFileAttributeDirectory) &&
               !(newattr & kNtFileAttributeDirectory)) {
      return enotdir();  // old is directory, but new isn't a directory
    } else if ((oldattr & kNtFileAttributeDirectory) &&
               (newattr & kNtFileAttributeDirectory)) {
      // both old and new are directories
      if (!__imp_RemoveDirectoryW(newpath16) &&
          GetLastError() == kNtErrorDirNotEmpty) {
        return enotempty();
      }
    }
  }

  if (MoveFileEx(oldpath16, newpath16, kNtMovefileReplaceExisting)) {
    return 0;
  } else {
    return __fix_enotdir3(-1, oldpath16, newpath16);
  }
}
