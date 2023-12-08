/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns current working directory, e.g.
 *
 *     const char *dirname = gc(getcwd(0,0)); // if malloc is linked
 *     const char *dirname = getcwd(alloca(PATH_MAX),PATH_MAX);
 *
 * @param buf is where UTF-8 NUL-terminated path string gets written,
 *     which may be NULL to ask this function to malloc a buffer
 * @param size is number of bytes available in buf, e.g. PATH_MAX+1,
 *     which may be 0 if buf is NULL
 * @return buf containing system-normative path or NULL w/ errno
 * @raise EACCES if the current directory path couldn't be accessed
 * @raise ERANGE if `size` wasn't big enough for path and nul byte
 * @raise ENOMEM on failure to allocate the requested buffer
 * @raise EINVAL if `size` is zero and `buf` is non-null
 * @raise EFAULT if `buf` points to invalid memory
 */
char *getcwd(char *buf, size_t size) {

  // setup memory per api
  char *path;
  if (buf) {
    path = buf;
    if (!size) {
      einval();
      return 0;
    }
  } else {
    if (!size) size = 4096;
    if (!(path = malloc(size))) {
      return 0;  // enomem
    }
  }

  // invoke the real system call
  int got = __getcwd(path, size);
  if (got == -1) {
    if (path != buf) {
      free(path);
    }
    return 0;  // eacces, erange, or efault
  }

  // cut allocation down to size
  if (path != buf) {
    char *p;
    if ((p = realloc(path, strlen(path) + 1))) {
      path = p;
    }
  }

  // return result
  return path;
}
