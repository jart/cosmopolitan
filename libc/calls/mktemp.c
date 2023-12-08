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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/at.h"
#include "libc/temp.h"

/**
 * Generates temporary filename.
 *
 *     char tmp[14] = "/tmp/hiXXXXXX";
 *     puts(mkstemp(tmp));
 *
 * @param template is mutated to replace last six X's with rng
 * @raise EINVAL if `template` didn't end with `XXXXXX`
 * @return always `template` which on success will have its XXXXXX bytes
 *     mutated to a random value, otherwise on error it'll hold an empty
 *     string and `errno` will be set
 * @see mkstemp() if you you need a file descriptor
 * @see mkstemps() if you you need a file extension
 * @see openatemp() for one temp roller to rule them all
 * @see mkostemp() if you you need a `O_CLOEXEC`, `O_APPEND`, etc.
 * @cancelationpoint
 */
char *mktemp(char *template) {
  int fd;
  if ((fd = openatemp(AT_FDCWD, template, 0, 0, 0)) != -1) {
    unassert(!close(fd));
  } else {
    *template = 0;
  }
  return template;
}
