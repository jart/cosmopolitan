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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/paths.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

struct fiodgname_arg {
  int len;
  void *buf;
};

static int PtsName(int fd, char *buf, size_t size) {
  if (size < 9 + 12) return erange();
  if (_isptmaster(fd)) return -1;

  if (IsLinux()) {
    int pty;
    if (sys_ioctl(fd, TIOCGPTN, &pty)) return -1;
    buf[0] = '/', buf[1] = 'd', buf[2] = 'e', buf[3] = 'v';
    buf[4] = '/', buf[5] = 'p', buf[6] = 't', buf[7] = 's';
    buf[8] = '/', FormatInt32(buf + 9, pty);
    return 0;
  }

  if (IsFreebsd()) {
    struct fiodgname_arg fgn = {size - 5, buf + 5};
    buf[0] = '/', buf[1] = 'd';
    buf[2] = 'e', buf[3] = 'v';
    buf[4] = '/', buf[5] = 0;
    if (sys_ioctl(fd, FIODGNAME, &fgn) == -1) {
      if (errno == EINVAL) errno = ERANGE;
      return -1;
    }
    return 0;
  }

  if (IsXnu()) {
    char b2[128];
    if (sys_ioctl(fd, TIOCPTYGNAME, b2)) return -1;
    if (strlen(b2) + 1 > size) return erange();
    strcpy(buf, b2);
    return 0;
  }

  return enosys();
}

/**
 * Gets name subordinate pseudoteletypewriter.
 *
 * @return 0 on success, or errno on error
 */
errno_t ptsname_r(int fd, char *buf, size_t size) {
  int rc, e = errno;
  if (!PtsName(fd, buf, size)) {
    rc = 0;
  } else {
    rc = errno;
    errno = e;
  }
  return rc;
}
