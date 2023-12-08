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
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_close_nt(int fd, int fildes) {
  if (fd + 0u >= g_fds.n) return ebadf();
  struct Fd *f = g_fds.p + fd;
  switch (f->kind) {
    case kFdZip:
      return _weaken(__zipos_close)(fd);
    case kFdEmpty:
      return ebadf();
    case kFdFile:
      void sys_fcntl_nt_lock_cleanup(int);
      if (_weaken(sys_fcntl_nt_lock_cleanup)) {
        _weaken(sys_fcntl_nt_lock_cleanup)(fildes);
      }
      if ((f->flags & O_ACCMODE) != O_RDONLY &&
          GetFileType(f->handle) == kNtFileTypeDisk) {
        // Like Linux, closing a file on Windows doesn't guarantee it is
        // immediately synced to disk. But unlike Linux this could cause
        // subsequent operations, e.g. unlink() to break w/ access error
        FlushFileBuffers(f->handle);
      }
      break;
    case kFdEpoll:
      if (_weaken(sys_close_epoll_nt)) {
        return _weaken(sys_close_epoll_nt)(fd);
      }
      break;
    case kFdSocket:
      if (_weaken(sys_closesocket_nt)) {
        return _weaken(sys_closesocket_nt)(g_fds.p + fd);
      }
      break;
    default:
      break;
  }
  return CloseHandle(f->handle) ? 0 : __winerr();
}
