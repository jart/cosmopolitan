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
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/sysv/errfuns.h"

int64_t sys_lseek_metal(struct Fd *fd, int64_t offset, int whence) {
  size_t new_pos;
  struct MetalFile *file;
  switch (fd->kind) {
    case kFdFile:
      file = (struct MetalFile *)fd->handle;
      switch (whence) {
        case SEEK_SET:
          new_pos = offset;
          break;
        case SEEK_CUR:
          new_pos = file->pos + offset;
          break;
        case SEEK_END:
          new_pos = file->size + offset;
          break;
        default:
          return einval();
      }
      if (new_pos > file->size) return einval();
      file->pos = new_pos;
      return new_pos;
    default:
      return ebadf();
  }
}
