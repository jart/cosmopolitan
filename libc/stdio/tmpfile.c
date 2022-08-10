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
#include "libc/errno.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * Opens stream backed by anonymous file.
 *
 * We use $TMPDIR or /tmp to create a temporary file securely, which
 * will be unlink()'d before this function returns. The file content
 * will be released from disk once fclose() is called.
 *
 * @see mkostempsm(), kTmpPath
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
FILE *tmpfile(void) {
  FILE *f;
  unsigned x;
  int fd, i, j, e;
  char path[PATH_MAX], *p;
  p = path;
  p = stpcpy(p, kTmpPath);
  p = stpcpy(p, "tmp.");
  if (program_invocation_short_name &&
      strlen(program_invocation_short_name) < 128) {
    p = stpcpy(p, program_invocation_short_name);
    *p++ = '.';
  }
  for (i = 0; i < 10; ++i) {
    x = rand64();
    for (j = 0; j < 6; ++j) {
      p[j] = "0123456789abcdefghijklmnopqrstuvwxyz"[x % 36];
      x /= 36;
    }
    p[j] = 0;
    e = errno;
    if ((fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0600)) != -1) {
      unlink(path);
      if ((f = fdopen(fd, "w+"))) {
        return f;
      } else {
        close(fd);
        return 0;
      }
    } else if (errno == EEXIST) {
      errno = e;
    } else {
      break;
    }
  }
  return 0;
}
