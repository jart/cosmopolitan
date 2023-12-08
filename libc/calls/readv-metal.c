/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/vga/vga.internal.h"
#ifdef __x86_64__

ssize_t sys_readv_metal(int fd, const struct iovec *iov, int iovlen) {
  int i;
  size_t got, toto;
  struct MetalFile *file;
  switch (g_fds.p[fd].kind) {
    case kFdConsole:
      /*
       * The VGA teletypewriter code may wish to send out "status report"
       * escape sequences, in response to requests sent to it via write().
       * Read & return these if they are available.
       */
      if (_weaken(sys_readv_vga)) {
        ssize_t res = _weaken(sys_readv_vga)(g_fds.p + fd, iov, iovlen);
        if (res > 0) return res;
      }
      /* fall through */
    case kFdSerial:
      return sys_readv_serial(fd, iov, iovlen);
    case kFdFile:
      file = (struct MetalFile *)g_fds.p[fd].handle;
      for (toto = i = 0; i < iovlen && file->pos < file->size; ++i) {
        got = MIN(iov[i].iov_len, file->size - file->pos);
        if (got) memcpy(iov[i].iov_base, file->base, got);
        toto += got;
      }
      return toto;
    default:
      return ebadf();
  }
}

#endif /* __x86_64__ */
