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
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/errno.h"

textwindows int sys_chdir_nt16(char16_t path[hasatleast PATH_MAX],
                               uint32_t len) {

  // the win32 SetCurrentDirectory() API wants us to put back a trailing
  // slash that was removed earlier, when __mkntpath() normalized things
  if (len + 2 > PATH_MAX)
    return enametoolong();
  if (SetCurrentDirectory(path))
    return 0;

  // remap win32 error codes to unix
  uint32_t dwErrorCode;
  switch ((dwErrorCode = GetLastError())) {
    case kNtErrorFileNotFound:  // 2 a.k.a. ENOENT
      // it does this when the parent components exist but file didn't
      errno = ENOENT;
      break;
    case kNtErrorPathNotFound:  // 3 a.k.a. ENOTDIR
      // with Windows, this means that the file doesn't exist, because
      // a parent component didn't exist. however unix still considers
      // that to be an ENOENT error. on UNIX this only becomes ENOTDIR
      // if a parent component DID exist, but was not a real directory
      if (__hasregularparent(path)) {
        errno = ENOTDIR;
      } else {
        errno = ENOENT;
      }
      break;
    case kNtErrorDirectory:  // 267 a.k.a. EISDIR
      // it'll do this when try to chdir to a regular file that exists
      // it could also be an ELOOP error although not worth supporting
      errno = ENOTDIR;
      break;
    default:
      errno = __errno_windows2linux(dwErrorCode);
      break;
  }
  return -1;
}

textwindows int sys_chdir_nt(const char *path) {
  int len;
  char16_t path16[PATH_MAX];
  if ((len = __mkntpath(path, path16)) == -1)
    return -1;
  return sys_chdir_nt16(path16, len);
}
