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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/flock.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static textwindows int sys_fcntl_nt_lock(struct Fd *f, int cmd, uintptr_t arg) {
  struct flock *l;
  uint32_t flags, err;
  struct NtOverlapped ov;
  int64_t pos, off, len, size;
  struct NtByHandleFileInformation info;
  if (!GetFileInformationByHandle(f->handle, &info)) return __winerr();
  if (!SetFilePointerEx(f->handle, 0, &pos, SEEK_CUR)) return __winerr();
  l = (struct flock *)arg;
  len = l->l_len;
  off = l->l_start;
  size = (uint64_t)info.nFileSizeHigh << 32 | info.nFileSizeLow;
  switch (l->l_whence) {
    case SEEK_SET:
      break;
    case SEEK_CUR:
      off = pos + off;
      break;
    case SEEK_END:
      off = size - off;
      break;
    default:
      return einval();
  }
  if (!len) len = size - off;
  if (off < 0 || len < 0) return einval();
  offset2overlap(off, &ov);
  if (l->l_type == F_RDLCK || l->l_type == F_WRLCK) {
    flags = 0;
    if (cmd == F_SETLK) flags |= kNtLockfileFailImmediately;
    /* TODO: How can we make SQLite locks on Windows to work? */
    /* if (l->l_type == F_WRLCK) flags |= kNtLockfileExclusiveLock; */
    if (LockFileEx(f->handle, flags, 0, len, len >> 32, &ov)) {
      return 0;
    } else {
      err = GetLastError();
      if (err == kNtErrorLockViolation) err = EAGAIN;
      errno = err;
      return -1;
    }
  } else if (l->l_type == F_UNLCK) {
    if (UnlockFileEx(f->handle, 0, len, len >> 32, &ov)) {
      return 0;
    } else {
      err = GetLastError();
      if (err == kNtErrorNotLocked) {
        return 0;
      } else {
        errno = err;
        return -1;
      }
    }
  } else {
    return einval();
  }
}

textwindows int sys_fcntl_nt(int fd, int cmd, uintptr_t arg) {
  uint32_t flags;
  if (__isfdkind(fd, kFdFile) || __isfdkind(fd, kFdSocket)) {
    if (cmd == F_GETFL) {
      return g_fds.p[fd].flags & (O_ACCMODE | O_APPEND | O_ASYNC | O_DIRECT |
                                  O_NOATIME | O_NONBLOCK);
    } else if (cmd == F_SETFL) {
      /*
       * - O_APPEND doesn't appear to be tunable at cursory glance
       * - O_NONBLOCK might require we start doing all i/o in threads
       * - O_DSYNC / O_RSYNC / O_SYNC maybe if we fsync() everything
       */
      return einval();
    } else if (cmd == F_GETFD) {
      if (g_fds.p[fd].flags & O_CLOEXEC) {
        return FD_CLOEXEC;
      } else {
        return 0;
      }
    } else if (cmd == F_SETFD) {
      if (arg & FD_CLOEXEC) {
        g_fds.p[fd].flags |= O_CLOEXEC;
        return FD_CLOEXEC;
      } else {
        g_fds.p[fd].flags &= ~O_CLOEXEC;
        return 0;
      }
    } else if (cmd == F_SETLK || cmd == F_SETLKW) {
      return sys_fcntl_nt_lock(g_fds.p + fd, cmd, arg);
    } else {
      return einval();
    }
  } else {
    return ebadf();
  }
}
