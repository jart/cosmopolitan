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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/sysv/errfuns.h"

#define _LOCK_SH 0
#define _LOCK_EX kNtLockfileExclusiveLock
#define _LOCK_NB kNtLockfileFailImmediately
#define _LOCK_UN 8

textwindows int sys_flock_nt(int fd, int op) {
  int64_t h;
  struct NtByHandleFileInformation info;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  h = g_fds.p[fd].handle;
  struct NtOverlapped ov = {.hEvent = h};

  if (!GetFileInformationByHandle(h, &info)) {
    return __winerr();
  }

  if (op & _LOCK_UN) {
    if (op & ~_LOCK_UN) {
      return einval();
    }
    if (UnlockFileEx(h, 0, info.nFileSizeLow, info.nFileSizeHigh, &ov)) {
      return 0;
    } else {
      return -1;
    }
  }

  if (op & ~(_LOCK_SH | _LOCK_EX | _LOCK_NB)) {
    return einval();
  }

  if (LockFileEx(h, op, 0, info.nFileSizeLow, info.nFileSizeHigh, &ov)) {
    return 0;
  } else {
    return -1;
  }
}
