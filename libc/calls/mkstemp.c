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
#include "libc/sysv/consts/at.h"
#include "libc/temp.h"

/**
 * Creates temporary file name and file descriptor, e.g.
 *
 *     char path[] = "/tmp/foo.XXXXXX";
 *     int fd = mkstemp(path);
 *     printf("%s is opened as %d\n", path, fd);
 *
 * @param template is mutated to replace last six X's with rng
 * @return open file descriptor r + w exclusive or -1 w/ errno
 * @raise EINVAL if `template` didn't end with `XXXXXX`
 * @see openatemp() for one temp roller to rule them all
 * @see mkostemp() if you you need a `O_CLOEXEC`, `O_APPEND`, etc.
 * @see mkstemps() if you you need a suffix
 * @see mktemp() if you don't need an fd
 * @see tmpfd() if you don't need a path
 * @cancelationpoint
 */
int mkstemp(char *template) {
  return openatemp(AT_FDCWD, template, 0, 0, 0);
}

__weak_reference(mkstemp, mkstemp64);
