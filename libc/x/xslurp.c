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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads entire file into memory.
 *
 * @return NUL-terminated malloc'd contents, or NULL w/ errno
 * @note this is uninterruptible
 */
void *xslurp(const char *path, size_t *opt_out_size) {
  int fd;
  size_t i, got;
  char *res, *p;
  ssize_t rc, size;
  res = NULL;
  if ((fd = open(path, O_RDONLY)) != -1) {
    if ((size = getfiledescriptorsize(fd)) != -1 &&
        (res = memalign(PAGESIZE, size + 1))) {
      if (size > 2 * 1024 * 1024) {
        fadvise(fd, 0, size, MADV_SEQUENTIAL);
      }
      for (i = 0; i < size; i += got) {
      TryAgain:
        if ((rc = pread(fd, res + i, size - i, i)) != -1) {
          if (!(got = rc)) {
            if (getfiledescriptorsize(fd) == -1) {
              abort();  // TODO(jart): what is this
            }
          }
        } else if (errno == EINTR) {
          goto TryAgain;
        } else {
          free(res);
          res = NULL;
          break;
        }
      }
      if (res) {
        if (opt_out_size) {
          *opt_out_size = size;
        }
        res[i] = '\0';
      }
    }
    close(fd);
  }
  return res;
}
