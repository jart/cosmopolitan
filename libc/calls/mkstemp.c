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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates temporary file name and file descriptor.
 *
 * The best way to construct your path template is:
 *
 *     char path[PATH_MAX+1];
 *     strlcat(path, kTmpDir, sizeof(path));
 *     strlcat(path, "sauce.XXXXXX", sizeof(path));
 *
 * This usage pattern makes mkstemp() equivalent to tmpfd():
 *
 *     int fd;
 *     fd = mkstemp(path);
 *     unlink(path);
 *
 * This usage pattern makes mkstemp() equivalent to mktemp():
 *
 *     close(mkstemp(path));
 *     puts(path);
 *
 * @param template is mutated to replace last six X's with rng
 * @return open file descriptor r + w exclusive or -1 w/ errno
 * @raise EINVAL if `template` didn't end with `XXXXXX`
 * @see tmpfd() if you don't need a path
 */
int mkstemp(char *template) {
  uint64_t w;
  int i, n, e, fd;
  if ((n = strlen(template)) < 6 ||
      READ16LE(template + n - 2) != READ16LE("XX") ||
      READ32LE(template + n - 6) != READ32LE("XXXX")) {
    return einval();
  }
  for (;;) {
    w = _rand64();
    for (i = 0; i < 6; ++i) {
      template[n - 6 + i] = "0123456789abcdefghijklmnopqrstuvwxyz"[w % 36];
      w /= 36;
    }
    e = errno;
    if ((fd = open(template,
                   O_RDWR | O_CREAT | O_EXCL | (IsWindows() ? 0x00410000 : 0),
                   0600)) != -1) {
      return fd;
    } else if (errno == EEXIST) {
      errno = e;
    } else {
      template[0] = 0;
      return fd;
    }
  }
}
