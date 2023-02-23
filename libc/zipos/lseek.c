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
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Changes current position of zip file handle.
 *
 * @param offset is the relative byte count
 * @param whence can be SEEK_SET, SEEK_CUR, or SEEK_END
 * @return new position relative to beginning, or -1 on error
 * @asyncsignalsafe
 */
int64_t __zipos_lseek(struct ZiposHandle *h, int64_t offset, unsigned whence) {
  int64_t rc;
  pthread_mutex_lock(&h->lock);
  switch (whence) {
    case SEEK_SET:
      rc = offset;
      break;
    case SEEK_CUR:
      rc = h->pos + offset;
      break;
    case SEEK_END:
      rc = h->size - offset;
      break;
    default:
      rc = -1;
      break;
  }
  if (rc >= 0) {
    h->pos = rc;
  } else {
    rc = einval();
  }
  pthread_mutex_unlock(&h->lock);
  return rc;
}
