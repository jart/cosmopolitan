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
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"

/**
 * Writes data to file.
 *
 * @param size can be -1 to strlen(data)
 * @return 0 on success or -1 w/ errno
 * @note this is uninterruptible
 */
int xbarf(const char *path, const void *data, size_t size) {
  ssize_t rc;
  int fd, res;
  const char *p;
  size_t i, wrote;
  res = 0;
  p = data;
  if (size == -1) size = data ? strlen(data) : 0;
  if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)) != -1) {
    if (ftruncate(fd, size) != -1) {
      if (size > 2 * 1024 * 1024) {
        fadvise(fd, 0, size, MADV_SEQUENTIAL);
      }
      for (i = 0; i < size; i += wrote) {
      TryAgain:
        if ((rc = pwrite(fd, p + i, size - i, i)) != -1) {
          wrote = rc;
        } else if (errno == EINTR) {
          goto TryAgain;
        } else {
          res = -1;
          break;
        }
      }
    } else {
      res = -1;
    }
    close(fd);
  } else {
    res = -1;
  }
  return res;
}
