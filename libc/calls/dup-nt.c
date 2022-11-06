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
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

// Implements dup(), dup2(), dup3(), and F_DUPFD for Windows.
textwindows int sys_dup_nt(int oldfd, int newfd, int flags, int start) {
  int64_t rc, proc, handle;
  _unassert(!(flags & ~O_CLOEXEC));

  __fds_lock();

  if (!__isfdopen(oldfd) || newfd < -1 ||
      (g_fds.p[oldfd].kind != kFdFile && g_fds.p[oldfd].kind != kFdSocket &&
       g_fds.p[oldfd].kind != kFdConsole)) {
    __fds_unlock();
    return ebadf();
  }

  // allocate a new file descriptor
  if (newfd == -1) {
    if ((newfd = __reservefd_unlocked(start)) == -1) {
      __fds_unlock();
      return -1;
    }
  } else {
    if (__ensurefds_unlocked(newfd) == -1) {
      __fds_unlock();
      return -1;
    }
    if (g_fds.p[newfd].kind) {
      sys_close_nt(g_fds.p + newfd, newfd);
    }
  }

  handle = g_fds.p[oldfd].handle;
  proc = GetCurrentProcess();

  if (DuplicateHandle(proc, handle, proc, &g_fds.p[newfd].handle, 0, true,
                      kNtDuplicateSameAccess)) {
    g_fds.p[newfd].kind = g_fds.p[oldfd].kind;
    g_fds.p[newfd].mode = g_fds.p[oldfd].mode;
    g_fds.p[newfd].flags = g_fds.p[oldfd].flags & ~O_CLOEXEC;
    if (flags & O_CLOEXEC) g_fds.p[newfd].flags |= O_CLOEXEC;
    if (g_fds.p[oldfd].kind == kFdSocket && _weaken(_dupsockfd)) {
      g_fds.p[newfd].extra =
          (intptr_t)_weaken(_dupsockfd)((struct SockFd *)g_fds.p[oldfd].extra);
    } else {
      g_fds.p[newfd].extra = g_fds.p[oldfd].extra;
    }
    rc = newfd;
  } else {
    __releasefd(newfd);
    rc = __winerr();
  }

  __fds_unlock();
  return rc;
}
