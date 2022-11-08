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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/wincrash.internal.h"
#include "libc/errno.h"
#include "libc/intrin/kmalloc.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/backtrace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

struct FileLock {
  struct FileLock *next;
  int64_t off;
  int64_t len;
  int fd;
  bool exc;
};

struct FileLocks {
  pthread_mutex_t mu;
  struct FileLock *list;
  struct FileLock *free;
};

static struct FileLocks g_locks;

static textwindows struct FileLock *NewFileLock(void) {
  struct FileLock *fl;
  if (g_locks.free) {
    fl = g_locks.free;
    g_locks.free = fl->next;
  } else {
    fl = kmalloc(sizeof(*fl));
  }
  bzero(fl, sizeof(*fl));
  fl->next = g_locks.list;
  g_locks.list = fl;
  return fl;
}

static textwindows void FreeFileLock(struct FileLock *fl) {
  fl->next = g_locks.free;
  g_locks.free = fl;
}

static textwindows bool OverlapsFileLock(struct FileLock *fl, int64_t off,
                                         int64_t len) {
  uint64_t BegA, EndA, BegB, EndB;
  BegA = off;
  EndA = off + (len - 1);
  BegB = fl->off;
  EndB = fl->off + (fl->len - 1);
  return MAX(BegA, BegB) < MIN(EndA, EndB);
}

static textwindows bool EncompassesFileLock(struct FileLock *fl, int64_t off,
                                            int64_t len) {
  return off <= fl->off && fl->off + fl->len <= off + len;
}

static textwindows bool EqualsFileLock(struct FileLock *fl, int64_t off,
                                       int64_t len) {
  return fl->off == off && off + len == fl->off + fl->len;
}

_Hide textwindows void sys_fcntl_nt_lock_cleanup(int fd) {
  struct FileLock *fl, *ft, **flp;
  pthread_mutex_lock(&g_locks.mu);
  for (flp = &g_locks.list, fl = *flp; fl;) {
    if (fl->fd == fd) {
      *flp = fl->next;
      ft = fl->next;
      FreeFileLock(fl);
      fl = ft;
    } else {
      flp = &fl->next;
      fl = *flp;
    }
  }
  pthread_mutex_unlock(&g_locks.mu);
}

static textwindows int sys_fcntl_nt_lock(struct Fd *f, int fd, int cmd,
                                         uintptr_t arg) {
  int e;
  struct flock *l;
  uint32_t flags, err;
  struct FileLock *fl, *ft, **flp;
  int64_t pos, off, len, end, size;

  l = (struct flock *)arg;
  len = l->l_len;
  off = l->l_start;

  switch (l->l_whence) {
    case SEEK_SET:
      break;
    case SEEK_CUR:
      pos = 0;
      if (SetFilePointerEx(f->handle, 0, &pos, SEEK_CUR)) {
        off = pos + off;
      } else {
        return __winerr();
      }
      break;
    case SEEK_END:
      off = INT64_MAX - off;
      break;
    default:
      return einval();
  }

  if (!len) {
    len = INT64_MAX - off;
  }

  if (off < 0 || len < 0 || __builtin_add_overflow(off, len, &end)) {
    return einval();
  }

  bool32 ok;
  struct NtOverlapped ov = {.hEvent = f->handle,
                            .Pointer = (void *)(uintptr_t)off};

  if (l->l_type == F_RDLCK || l->l_type == F_WRLCK) {

    if (cmd == F_SETLK || cmd == F_SETLKW) {
      // make it possible to transition read locks to write locks
      for (flp = &g_locks.list, fl = *flp; fl;) {
        if (fl->fd == fd) {
          if (EqualsFileLock(fl, off, len)) {
            if (fl->exc == l->l_type == F_WRLCK) {
              // we already have this lock
              return 0;
            } else {
              // unlock our read lock and acquire write lock below
              if (UnlockFileEx(f->handle, 0, len, len >> 32, &ov)) {
                *flp = fl->next;
                ft = fl->next;
                FreeFileLock(fl);
                fl = ft;
                continue;
              } else {
                return -1;
              }
            }
            break;
          } else if (OverlapsFileLock(fl, off, len)) {
            return enotsup();
          }
        }
        flp = &fl->next;
        fl = *flp;
      }
    }

    // return better information on conflicting locks if possible
    if (cmd == F_GETLK) {
      for (fl = g_locks.list; fl; fl = fl->next) {
        if (fl->fd == fd &&  //
            OverlapsFileLock(fl, off, len) &&
            (l->l_type == F_WRLCK || !fl->exc)) {
          l->l_whence = SEEK_SET;
          l->l_start = fl->off;
          l->l_len = fl->len;
          l->l_type == fl->exc ? F_WRLCK : F_RDLCK;
          l->l_pid = getpid();
          return 0;
        }
      }
    }

    flags = 0;
    if (cmd != F_SETLKW) {
      // TODO(jart): we should use expo backoff in wrapper function
      //             should not matter since sqlite doesn't need it
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
          return -1;
        }
      } else {
        l->l_pid = -1;
        ok = true;
      }
    } else if (ok) {
      fl = NewFileLock();
      fl->off = off;
      fl->len = len;
      fl->exc = l->l_type == F_WRLCK;
      fl->fd = fd;
    }
    return ok ? 0 : -1;
  }

  if (l->l_type == F_UNLCK) {
    if (cmd == F_GETLK) return einval();

    // allow a big range to unlock many small ranges
    for (flp = &g_locks.list, fl = *flp; fl;) {
      if (fl->fd == fd && EncompassesFileLock(fl, off, len)) {
        struct NtOverlapped ov = {.hEvent = f->handle,
                                  .Pointer = (void *)(uintptr_t)fl->off};
        if (UnlockFileEx(f->handle, 0, fl->len, fl->len >> 32, &ov)) {
          *flp = fl->next;
          ft = fl->next;
          FreeFileLock(fl);
          fl = ft;
        } else {
          return -1;
        }
      } else {
        flp = &fl->next;
        fl = *flp;
      }
    }

    // win32 won't let us carve up existing locks
    int overlap_count = 0;
    for (fl = g_locks.list; fl; fl = fl->next) {
      if (fl->fd == fd &&  //
          OverlapsFileLock(fl, off, len)) {
        ++overlap_count;
      }
    }

    // try to handle the carving cases needed by sqlite
    if (overlap_count == 1) {
      for (fl = g_locks.list; fl; fl = fl->next) {
        if (fl->fd == fd &&          //
            off <= fl->off &&        //
            off + len >= fl->off &&  //
            off + len < fl->off + fl->len) {
          // cleave left side of lock
          struct NtOverlapped ov = {.hEvent = f->handle,
                                    .Pointer = (void *)(uintptr_t)fl->off};
          if (!UnlockFileEx(f->handle, 0, fl->len, fl->len >> 32, &ov)) {
            return -1;
          }
          fl->len = (fl->off + fl->len) - (off + len);
          fl->off = off + len;
          ov.Pointer = (void *)(uintptr_t)fl->off;
          if (!LockFileEx(f->handle, kNtLockfileExclusiveLock, 0, fl->len,
                          fl->len >> 32, &ov)) {
            return -1;
          }
          return 0;
        }
      }
    }

    if (overlap_count) {
      return enotsup();
    }

    return 0;
  }

  return einval();
}

static textwindows int sys_fcntl_nt_dupfd(int fd, int cmd, int start) {
  if (start < 0) return einval();
  return sys_dup_nt(fd, -1, (cmd == F_DUPFD_CLOEXEC ? O_CLOEXEC : 0), start);
}

textwindows int sys_fcntl_nt(int fd, int cmd, uintptr_t arg) {
  int rc;
  uint32_t flags;
  if (__isfdkind(fd, kFdFile) || __isfdkind(fd, kFdSocket)) {
    if (cmd == F_GETFL) {
      rc = g_fds.p[fd].flags &
           (O_ACCMODE | O_APPEND | O_ASYNC | O_DIRECT | O_NOATIME | O_NONBLOCK |
            O_RANDOM | O_SEQUENTIAL);
    } else if (cmd == F_SETFL) {
      // O_APPEND doesn't appear to be tunable at cursory glance
      // O_NONBLOCK might require we start doing all i/o in threads
      // O_DSYNC / O_RSYNC / O_SYNC maybe if we fsync() everything
      // O_DIRECT | O_RANDOM | O_SEQUENTIAL | O_NDELAY possible but
      // not worth it.
      rc = enosys();
    } else if (cmd == F_GETFD) {
      if (g_fds.p[fd].flags & O_CLOEXEC) {
        rc = FD_CLOEXEC;
      } else {
        rc = 0;
      }
    } else if (cmd == F_SETFD) {
      if (arg & FD_CLOEXEC) {
        g_fds.p[fd].flags |= O_CLOEXEC;
      } else {
        g_fds.p[fd].flags &= ~O_CLOEXEC;
      }
      rc = 0;
    } else if (cmd == F_SETLK || cmd == F_SETLKW || cmd == F_GETLK) {
      pthread_mutex_lock(&g_locks.mu);
      rc = sys_fcntl_nt_lock(g_fds.p + fd, fd, cmd, arg);
      pthread_mutex_unlock(&g_locks.mu);
    } else if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) {
      rc = sys_fcntl_nt_dupfd(fd, cmd, arg);
    } else {
      rc = einval();
    }
  } else {
    rc = ebadf();
  }
  return rc;
}
