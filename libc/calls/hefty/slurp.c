/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/arraylist2.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"

static size_t getfilesize(int fd) {
  struct stat st;
  st.st_size = 0;
  fstat(fd, &st);
  return st.st_size;
}

static size_t smudgefilesize(size_t size) {
  return roundup(size, PAGESIZE) + PAGESIZE;
}

/**
 * Reads entire file into memory.
 *
 * This function is fantastic for being lazy. It may waste space if the
 * file is large, but it's implemented in a conscientious way that won't
 * bomb the systemwide page cache. It means performance too, naturally.
 *
 * @return contents which must be free()'d or NULL w/ errno
 */
char *slurp(const char *path, size_t *opt_out_readlength) {
  int fd;
  ssize_t rc;
  char *res, *p;
  size_t i, n, got, want;
  res = NULL;
  if ((fd = open(path, O_RDONLY)) == -1) goto Failure;
  n = getfilesize(fd);
  /* TODO(jart): Fix this, it's totally broken */
  if (!(res = valloc(smudgefilesize(n)))) goto Failure;
  if (n > FRAMESIZE) fadvise(fd, 0, n, MADV_SEQUENTIAL);
  i = 0;
  for (;;) {
    want = smudgefilesize(n - i);
  TryAgain:
    if ((rc = read(fd, &res[i], want)) == -1) {
      if (errno == EINTR) goto TryAgain;
      goto Failure;
    }
    got = (size_t)rc;
    if (i + 1 >= n) {
      if ((p = realloc(res, smudgefilesize((n += n >> 1))))) {
        res = p;
      } else {
        goto Failure;
      }
    }
    i += got;
    if (got == 0) break;
    if (got > want) abort();
  }
  if (opt_out_readlength) *opt_out_readlength = i;
  res[i] = '\0';
  close(fd);
  return res;
Failure:
  free(res);
  close(fd);
  return NULL;
}
