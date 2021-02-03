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
#include "libc/alg/alg.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

static void *filecmp$mmap(int fd, size_t size) {
  return size ? mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0) : NULL;
}

/**
 * Compares contents of files with memcmp().
 *
 * @return ≤0, 0, or ≥0 based on comparison; or ≠0 on error, in which
 *     case we make our best effort to sift failing filenames rightward,
 *     and errno can be set to 0 beforehand to differentiate errors
 */
int filecmp(const char *pathname1, const char *pathname2) {
  int res, olderr;
  int fd1 = -1;
  int fd2 = -1;
  char *addr1 = MAP_FAILED;
  char *addr2 = MAP_FAILED;
  size_t size1 = 0;
  size_t size2 = 0;
  if ((fd1 = open(pathname1, O_RDONLY)) != -1 &&
      (fd2 = open(pathname2, O_RDONLY)) != -1 &&
      (size1 = getfiledescriptorsize(fd1)) != -1 &&
      (size2 = getfiledescriptorsize(fd2)) != -1 &&
      (addr1 = filecmp$mmap(fd1, size1)) != MAP_FAILED &&
      (addr2 = filecmp$mmap(fd2, size2)) != MAP_FAILED) {
    olderr = errno;
    madvise(addr1, size1, MADV_WILLNEED | MADV_SEQUENTIAL);
    madvise(addr2, size2, MADV_WILLNEED | MADV_SEQUENTIAL);
    errno = olderr;
    res = memcmp(addr1, addr2, min(size1, size2));
    if (!res && size1 != size2) {
      char kNul = '\0';
      if (size1 > size2) {
        res = cmpub(addr1 + size2, &kNul);
      } else {
        res = cmpub(addr2 + size1, &kNul);
      }
    }
  } else {
    res = cmpuq(&fd1, &fd2) | 1;
  }
  olderr = errno;
  munmap(addr1, size1);
  munmap(addr2, size2);
  close(fd1);
  close(fd2);
  errno = olderr;
  return res;
}
