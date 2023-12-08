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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/sysv/consts/s.h"

/**
 * Returns true if file descriptor is backed by character i/o.
 *
 * This function is equivalent to:
 *
 *     struct stat st;
 *     return stat(path, &st) != -1 && S_ISCHR(st.st_mode);
 *
 * Except faster and with fewer dependencies.
 *
 * @see isregularfile(), isdirectory(), issymlink(), fileexists()
 */
bool32 ischardev(int fd) {
  int e;
  union metastat st;
  if (__isfdkind(fd, kFdZip)) {
    return false;
  } else if (IsMetal()) {
    return true;
  } else if (!IsWindows()) {
    e = errno;
    if (__sys_fstat(fd, &st) != -1) {
      return S_ISCHR(METASTAT(st, st_mode));
    } else {
      errno = e;
      return false;
    }
  } else {
    return __isfdkind(fd, kFdConsole) || __isfdkind(fd, kFdDevNull) ||
           (__isfdkind(fd, kFdFile) &&
            GetFileType(g_fds.p[fd].handle) == kNtFileTypeChar);
  }
}
