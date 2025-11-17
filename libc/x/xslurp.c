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
#include "libc/mem/mem.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"

/**
 * Reads entire file into memory.
 * @return NUL-terminated malloc'd contents, or NULL w/ errno
 */
void *xslurp(const char *path, size_t *opt_out_size) {

  // open file
  int fd;
  if ((fd = open(path, O_RDONLY)) == -1)
    return 0;

  // setup growable array
  char *data = 0;
  size_t size = 0;
  size_t capacity = 0;
  size_t granularity = 4096;

  // read from fd until eof
  for (;;) {
    if (capacity - size < granularity) {
      capacity += granularity;
      char *p = realloc(data, capacity + 1);
      if (!p) {
        free(data);
        close(fd);
        return 0;
      }
      data = p;
    }
    ssize_t rc = read(fd, data + size, capacity - size);
    if (rc == -1) {
      free(data);
      close(fd);
      return 0;
    }
    if (!rc) {
      realloc_in_place(data, size + 1);
      data[size] = 0;
      close(fd);
      if (opt_out_size)
        *opt_out_size = size;
      return data;
    }
    size += rc;
  }
}
