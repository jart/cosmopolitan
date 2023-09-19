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
#include "libc/calls/calls.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"

static int64_t GetPosition(struct ZiposHandle *h, int whence) {
  switch (whence) {
    case SEEK_SET:
      return 0;
    case SEEK_CUR:
      return h->pos;
    case SEEK_END:
      return h->size;
    default:
      return einval();
  }
}

static int64_t Seek(struct ZiposHandle *h, int64_t offset, int whence) {
  int64_t pos;
  if (!ckd_add(&pos, GetPosition(h, whence), offset)) {
    if (pos >= 0) {
      return pos;
    } else {
      return einval();
    }
  } else {
    return eoverflow();
  }
}

/**
 * Changes current position of zip file handle.
 *
 * @param offset is the relative byte count
 * @param whence can be SEEK_SET, SEEK_CUR, or SEEK_END
 * @return new position relative to beginning, or -1 on error
 * @asyncsignalsafe
 */
int64_t __zipos_seek(struct ZiposHandle *h, int64_t offset, unsigned whence) {
  int64_t pos;
  if ((pos = Seek(h, offset, whence)) != -1) {
    h->pos = pos;
  }
  return pos;
}
