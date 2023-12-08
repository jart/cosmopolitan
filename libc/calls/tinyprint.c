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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/runtime/runtime.h"

#define ASAN_ERROR "error: tinyprint() passed invalid memory, or missing NULL\n"

static bool tinyflush(int fd, char *buf, size_t n, ssize_t *toto) {
  ssize_t rc;
  if ((rc = write(fd, buf, n)) != -1) {
    *toto += rc;
    return false;
  } else {
    if (!*toto) {
      --*toto;
    }
    return true;
  }
}

/**
 * Writes strings to file descriptor, e.g.
 *
 *     tinyprint(1, "hello", " ", "world", "\n", NULL);
 *
 * This function is intended so that small command line programs (e.g.
 * build utilities) can print succinct error messages automatically to
 * standard io without linking too much binary footprint.
 *
 * @param fd is file descriptor
 * @param s is NUL-terminated string, or NULL
 * @return bytes written on success, or -1 w/ errno
 */
ssize_t tinyprint(int fd, const char *s, ...) {
  int c;
  size_t n;
  va_list va;
  ssize_t toto;
  char buf[512];
  BLOCK_CANCELATION;
  va_start(va, s);
  for (toto = n = 0; s; s = va_arg(va, const char *)) {
    if (IsAsan() && !__asan_is_valid_str(s)) {
      write(2, ASAN_ERROR, sizeof(ASAN_ERROR) - 1);
      abort();
    }
    while ((c = *s++)) {
      buf[n++] = c;
      if (n == sizeof(buf)) {
        if (tinyflush(fd, buf, n, &toto)) {
          return toto;
        }
        n = 0;
      }
    }
  }
  va_end(va);
  tinyflush(fd, buf, n, &toto);
  ALLOW_CANCELATION;
  return toto;
}
