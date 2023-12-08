/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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

#include "libc/calls/cachestat.h"
#include "libc/intrin/strace.internal.h"

int sys_cachestat(int, struct cachestat_range *, struct cachestat *, uint32_t);

/**
 * Query the page cache statistics of a file.
 *
 * @param fd The open file descriptor to retrieve statistics from.
 * @param cstat_range The byte range in `fd` to query. When `len > 0`, the range
 * is `[off..off + len]`. When `len` == 0, the range is from `off` to the end of
 * `fd`.
 * @param cstat The structure where page cache statistics are stored.
 * @param flags Currently unused, and must be set to `0`.
 * @return 0 on success, or -1 w/ errno.
 * @raise EFAULT if `cstat_range` or `cstat` points to invalid memory
 * @raise EINVAL if `flags` is nonzero
 * @raise EBADF  if `fd` is negative or not open
 * @raise EOPNOTSUPP if `fd` refers to a hugetlbfs file
 * @raise ENOSYS if not Linux 6.5
 */
int cachestat(int fd, struct cachestat_range *cstat_range,
              struct cachestat *cstat, uint32_t flags) {
  int rc;
  rc = sys_cachestat(fd, cstat_range, cstat, flags);
  STRACE("cachestat(%d, %p, %p, %#x) → %d% m", fd, cstat_range, cstat, flags);
  return rc;
}