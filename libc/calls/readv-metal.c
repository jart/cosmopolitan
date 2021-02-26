/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/macros.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

ssize_t sys_readv_metal(struct Fd *fd, const struct iovec *iov, int iovlen) {
  int i;
  size_t got, toto;
  struct MetalFile *file;
  switch (fd->kind) {
    case kFdSerial:
      return sys_readv_serial(fd, iov, iovlen);
    case kFdFile:
      file = (struct MetalFile *)fd->handle;
      for (toto = i = 0; i < iovlen && file->pos < file->size; ++i) {
        got = MIN(iov[i].iov_len, file->size - file->pos);
        memcpy(iov[i].iov_base, file->base, got);
        toto += got;
      }
      return toto;
    default:
      return ebadf();
  }
}
