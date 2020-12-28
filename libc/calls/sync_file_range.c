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
#include "libc/errno.h"

/**
 * Flushes subset of file to disk.
 *
 * @param offset is page rounded
 * @param bytes is page rounded; 0 means until EOF
 * @param flags can have SYNC_FILE_RANGE_{WAIT_BEFORE,WRITE,WAIT_AFTER}
 * @note Linux documentation says this call is "dangerous"; for highest
 *     assurance of data recovery after crash, consider fsync() on both
 *     file and directory
 * @see fsync(), fdatasync(), PAGESIZE
 */
int sync_file_range(int fd, int64_t offset, int64_t bytes, unsigned flags) {
  int rc, olderr;
  olderr = errno;
  if ((rc = sync_file_range$sysv(fd, offset, bytes, flags)) != -1 ||
      errno != ENOSYS) {
    return rc;
  } else {
    errno = olderr;
    return fdatasync(fd);
  }
}
