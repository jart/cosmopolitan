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
 *     char path[] = "/tmp/foo.XXXXXX.txt";
 *     int fd = mkstemps(path, 4);
 *     printf("%s is opened as %d\n", path, fd);
 *
 * @param template is mutated to replace last six X's with rng
 * @param suffixlen may be nonzero to permit characters after the "XXXXXX"
 * @return open file descriptor r + w exclusive or -1 w/ errno
 * @raise EINVAL if `template` (less the `suffixlen` region) didn't
 *     end with the string "XXXXXXX"
 * @see mkostemp() if you you need a `O_CLOEXEC`, `O_APPEND`, etc.
 * @see openatemp() for one temp roller to rule them all
 * @see mkstemp() if you don't need `suffixlen`
 * @see mktemp() if you don't need an fd
 * @see tmpfd() if you don't need a path
 * @cancelationpoint
 */
int mkstemps(char *template, int suffixlen) {
  return openatemp(AT_FDCWD, template, suffixlen, 0, 0);
}

__weak_reference(mkstemps, mkstemps64);
