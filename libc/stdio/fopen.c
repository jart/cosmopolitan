/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static const char *fixpathname(const char *pathname, int flags) {
  if ((flags & O_ACCMODE) == O_RDONLY && strcmp(pathname, "-") == 0) {
    return "/dev/stdin";
  } else if ((flags & O_ACCMODE) == O_WRONLY && strcmp(pathname, "-") == 0) {
    return "/dev/stdout";
  } else {
    return pathname;
  }
}

static int openpathname(const char *pathname, int flags, bool *out_noclose) {
  if ((flags & O_ACCMODE) == O_RDONLY && strcmp(pathname, "/dev/stdin") == 0) {
    *out_noclose = true;
    return fileno(stdin);
  } else if ((flags & O_ACCMODE) == O_WRONLY &&
             strcmp(pathname, "/dev/stdout") == 0) {
    *out_noclose = true;
    return fileno(stdout);
  } else {
    *out_noclose = false;
    return open(pathname, flags, 0666);
  }
}

/**
 * Opens file as stream object.
 *
 * @param pathname is a utf-8 ideally relative filename
 * @param mode is the string mode/flag DSL see fopenflags()
 * @return new object to be free'd by fclose() or NULL w/ errno
 * @note microsoft unilaterally deprecated this function lool
 */
FILE *fopen(const char *pathname, const char *mode) {
  FILE *f = 0;
  bool noclose;
  int fd, flags;
  if (!pathname) {
    efault();
    return 0;
  }
  flags = fopenflags(mode);
  pathname = fixpathname(pathname, flags);
  if ((fd = openpathname(pathname, flags, &noclose)) != -1) {
    if ((f = fdopen(fd, mode)) != NULL) {
      f->noclose = noclose;
    } else if (!noclose) {
      close(fd);
    }
  }
  return f;
}
