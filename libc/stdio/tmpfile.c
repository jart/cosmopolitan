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
#include "libc/stdio/stdio.h"
#include "libc/temp.h"

/**
 * Opens stream backed by anonymous file, e.g.
 *
 *     FILE *f;
 *     if (!(f = tmpfile())) {
 *       perror("tmpfile");
 *       exit(1);
 *     }
 *     // do stuff
 *     fclose(f);
 *
 * This creates a secure temporary file inside $TMPDIR. If it isn't
 * defined, then /tmp is used on UNIX and GetTempPath() is used on the
 * New Technology. This resolution of $TMPDIR happens once in a ctor.
 *
 * Once fclose() is called, the returned file is guaranteed to be
 * deleted automatically. On UNIX the file is unlink()'d before this
 * function returns. On the New Technology it happens upon fclose().
 *
 * On newer Linux only (c. 2013) it's possible to turn the anonymous
 * returned file back into a real file, by doing this:
 *
 *     linkat(AT_FDCWD, gc(xasprintf("/proc/self/fd/%d", fileno(f))),
 *            AT_FDCWD, "real.txt", AT_SYMLINK_FOLLOW)
 *
 * On the New Technology, temporary files created by this function
 * should have better performance, because `kNtFileAttributeTemporary`
 * asks the kernel to more aggressively cache and reduce i/o ops.
 *
 * Favor tmpfd() or tmpfile() over `open(O_TMPFILE)` because the latter
 * is Linux-only and will cause open() failures on all other platforms.
 *
 * @see tmpfd() if you don't want to link stdio/malloc
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @vforksafe
 */
FILE *tmpfile(void) {
  int fd;
  FILE *f;
  if ((fd = tmpfd()) != -1) {
    if ((f = fdopen(fd, "w+"))) {
      return f;
    } else {
      close(fd);
      return 0;
    }
  } else {
    return 0;
  }
}
