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
#include "libc/calls/internal.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/errfuns.h"

textwindows int flock$nt(int fd, int op) {
  struct NtOverlapped ov;
  struct NtByHandleFileInformation info;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  memset(&ov, 0, sizeof(ov));
  if (GetFileInformationByHandle(g_fds.p[fd].handle, &info) &&
      ((!(op & LOCK_UN) &&
        LockFileEx(g_fds.p[fd].handle, op, 0, info.nFileSizeLow,
                   info.nFileSizeHigh, &ov)) ||
       ((op & LOCK_UN) && UnlockFileEx(g_fds.p[fd].handle, 0, info.nFileSizeLow,
                                       info.nFileSizeHigh, &ov)))) {
    return 0;
  } else {
    return __winerr();
  }
}
