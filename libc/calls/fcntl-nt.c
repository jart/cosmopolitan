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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/wincrash.internal.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/log/backtrace.internal.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

bool __force_sqlite_to_work_until_we_can_fix_it;

static textwindows int sys_fcntl_nt_dupfd(int fd, int cmd, int start) {
  if (start < 0) return einval();
  return sys_dup_nt(fd, -1, (cmd == F_DUPFD_CLOEXEC ? O_CLOEXEC : 0), start);
}

static textwindows int sys_fcntl_nt_lock(struct Fd *f, int cmd, uintptr_t arg) {
  int e;
  struct flock *l;
  uint32_t flags, err;
  int64_t pos, off, len, size;
  struct NtByHandleFileInformation info;

  if (!GetFileInformationByHandle(f->handle, &info)) {
    return __winerr();
  }

  pos = 0;
  if (!SetFilePointerEx(f->handle, 0, &pos, SEEK_CUR)) {
    return __winerr();
  }

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

  if (!len) {
    len = size - off;
  }

  if (off < 0 || len < 0) {
    return einval();
  }

  bool32 ok;
  struct NtOverlapped ov = {.hEvent = f->handle,
                            .Pointer = (void *)(uintptr_t)off};

  if (l->l_type == F_RDLCK || l->l_type == F_WRLCK) {
    flags = 0;
    if (cmd != F_SETLKW) {
      flags |= kNtLockfileFailImmediately;
    }
    if (l->l_type == F_WRLCK) {
      flags |= kNtLockfileExclusiveLock;
    }
    ok = LockFileEx(f->handle, flags, 0, len, len >> 32, &ov);
    if (cmd == F_GETLK) {
      if (ok) {
        l->l_type = F_UNLCK;
        if (!UnlockFileEx(f->handle, 0, len, len >> 32, &ov)) {
          notpossible;
        }
      } else {
        l->l_pid = -1;
        ok = true;
      }
    }
    if (ok || __force_sqlite_to_work_until_we_can_fix_it) {
      return 0;
    } else {
      return -1;
    }
  }

  if (l->l_type == F_UNLCK) {
    if (cmd == F_GETLK) return einval();
    e = errno;
    if (UnlockFileEx(f->handle, 0, len, len >> 32, &ov)) {
      return 0;
    } else if (errno == ENOLCK) {
      errno = e;
      return 0;
    } else {
      return 0;
    }
  }

  return einval();
}

textwindows int sys_fcntl_nt(int fd, int cmd, uintptr_t arg) {
  uint32_t flags;
  if (__isfdkind(fd, kFdFile) || __isfdkind(fd, kFdSocket)) {
    if (cmd == F_GETFL) {
      return g_fds.p[fd].flags &
             (O_ACCMODE | O_APPEND | O_ASYNC | O_DIRECT | O_NOATIME |
              O_NONBLOCK | O_RANDOM | O_SEQUENTIAL);
    } else if (cmd == F_SETFL) {
      // O_APPEND doesn't appear to be tunable at cursory glance
      // O_NONBLOCK might require we start doing all i/o in threads
      // O_DSYNC / O_RSYNC / O_SYNC maybe if we fsync() everything
      // O_DIRECT | O_RANDOM | O_SEQUENTIAL | O_NDELAY possible but
      // not worth it.
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
    } else if (cmd == F_SETLK || cmd == F_SETLKW || cmd == F_GETLK) {
      return sys_fcntl_nt_lock(g_fds.p + fd, cmd, arg);
    } else if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) {
      return sys_fcntl_nt_dupfd(fd, cmd, arg);
    } else {
      return einval();
    }
  } else {
    return ebadf();
  }
}
