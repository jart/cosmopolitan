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
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

#define _O_TMPFILE 000020200000

/**
 * Returns file descriptor of open anonymous file, e.g.
 *
 *     int fd;
 *     if ((fd = tmpfd()) == -1) {
 *       perror("tmpfd");
 *       exit(1);
 *     }
 *     // do stuff
 *     close(f);
 *
 * This creates a secure temporary file inside $TMPDIR. If it isn't
 * defined, then /tmp is used on UNIX and GetTempPath() is used on the
 * New Technology. This resolution of $TMPDIR happens once in a ctor,
 * which is copied to the `kTmpDir` global.
 *
 * Once close() is called, the returned file is guaranteed to be deleted
 * automatically. On UNIX the file is unlink()'d before this function
 * returns. On the New Technology it happens upon close().
 *
 * On newer Linux only (c. 2013) it's possible to turn the anonymous
 * returned file back into a real file, by doing this:
 *
 *     linkat(AT_FDCWD, _gc(xasprintf("/proc/self/fd/%d", fd)),
 *            AT_FDCWD, "real.txt", AT_SYMLINK_FOLLOW)
 *
 * On the New Technology, temporary files created by this function
 * should have better performance, because `kNtFileAttributeTemporary`
 * asks the kernel to more aggressively cache and reduce i/o ops.
 *
 * The tmpfd() function should be favored over `open(O_TMPFILE)` because
 * the latter only works on Linux, and will cause open() failures on all
 * other platforms.
 *
 * @return file descriptor on success, or -1 w/ errno
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @see tmpfile() for stdio version
 * @cancellationpoint
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
int tmpfd(void) {
  FILE *f;
  unsigned x;
  int fd, i, j, e;
  char path[PATH_MAX], *p;
  e = errno;
  if (IsLinux() && (fd = open(kTmpPath, O_RDWR | _O_TMPFILE, 0600)) != -1) {
    return fd;
  }
  errno = e;
  p = path;
  p = stpcpy(p, kTmpPath);
  p = stpcpy(p, "tmp.");
  if (program_invocation_short_name &&
      strlen(program_invocation_short_name) < 128) {
    p = stpcpy(p, program_invocation_short_name);
    *p++ = '.';
  }
  for (i = 0; i < 10; ++i) {
    x = _rand64();
    for (j = 0; j < 6; ++j) {
      p[j] = "0123456789abcdefghijklmnopqrstuvwxyz"[x % 36];
      x /= 36;
    }
    p[j] = 0;
    e = errno;
    if ((fd = open(path,
                   O_RDWR | O_CREAT | O_EXCL | (IsWindows() ? _O_TMPFILE : 0),
                   0600)) != -1) {
      if (!IsWindows()) {
        if (unlink(path)) {
          notpossible;
        }
      }
      return fd;
    } else if (errno == EEXIST) {
      errno = e;
    } else {
      break;
    }
  }
  return -1;
}
