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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/fds.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

static textwindows int64_t GetPosition(struct Fd *f, int whence) {
  switch (whence) {
    case SEEK_SET:
      return 0;
    case SEEK_CUR:
      return f->cursor->shared->pointer;
    case SEEK_END: {
      struct NtByHandleFileInformation wst;
      if (!GetFileInformationByHandle(f->handle, &wst)) {
        return __winerr();
      }
      return (wst.nFileSizeHigh + 0ull) << 32 | wst.nFileSizeLow;
    }
    default:
      return einval();
  }
}

static textwindows int64_t Seek(struct Fd *f, int64_t offset, int whence) {
  int64_t pos;
  if ((pos = GetPosition(f, whence)) != -1) {
    if (!ckd_add(&pos, pos, offset)) {
      if (pos >= 0) {
        return pos;
      } else {
        return einval();
      }
    } else {
      return eoverflow();
    }
  } else {
    return -1;
  }
}

textwindows int64_t sys_lseek_nt(int fd, int64_t offset, int whence) {
  if (__isfdkind(fd, kFdDevNull) || __isfdkind(fd, kFdDevRandom)) {
    return offset;
  } else if (__isfdkind(fd, kFdFile)) {
    struct Fd *f = g_fds.p + fd;
    int filetype = GetFileType(f->handle);
    if (filetype != kNtFileTypePipe &&  //
        filetype != kNtFileTypeChar &&  //
        f->cursor->shared) {
      int64_t res;
      __cursor_lock(f->cursor);
      if ((res = Seek(f, offset, whence)) != -1)
        f->cursor->shared->pointer = res;
      __cursor_unlock(f->cursor);
      return res;
    } else {
      return espipe();
    }
  } else if (__isfdkind(fd, kFdSocket)) {
    return espipe();
  } else {
    return ebadf();
  }
}
