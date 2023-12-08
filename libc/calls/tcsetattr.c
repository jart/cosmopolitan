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
#include "libc/calls/internal.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/struct/termios.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/calls/termios.internal.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/alloca.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

void __on_tcsetattr(int);
int tcsetattr_nt(int, int, const struct termios *);

static const char *DescribeTcsa(char buf[12], int opt) {
  if (opt == TCSANOW) return "TCSANOW";
  if (opt == TCSADRAIN) return "TCSADRAIN";
  if (opt == TCSAFLUSH) return "TCSAFLUSH";
  FormatInt32(buf, opt);
  return buf;
}

static int tcsetattr_impl(int fd, int opt, const struct termios *tio) {
  if (fd < 0) {
    return einval();
  }

  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    return enotty();
  }

  if (0 <= fd && fd <= 2 && _weaken(__on_tcsetattr)) {
    static bool once;
    if (!once) {
      _weaken(__on_tcsetattr)(fd);
      once = true;
    }
  }

  if (IsAsan() && !__asan_is_valid(tio, sizeof(*tio))) {
    return efault();
  }

  if (IsMetal()) {
    return 0;
  }

  if (IsWindows()) {
    return tcsetattr_nt(fd, opt, tio);
  }

  if (IsLinux() || IsBsd()) {
    union metatermios mt;
    return sys_ioctl(fd, TCSETS + opt, __termios2host(&mt, tio));
  }

  return enosys();
}

/**
 * Sets struct on teletypewriter w/ drains and flushes.
 *
 * @param fd open file descriptor that isatty()
 * @param opt can be:
 *     - TCSANOW:                                   sets console settings
 *     - TCSADRAIN:              drains output, and sets console settings
 *     - TCSAFLUSH: drops input, drains output, and sets console settings
 * @return 0 on success, -1 w/ errno
 * @asyncsignalsafe
 */
int tcsetattr(int fd, int opt, const struct termios *tio) {
  int rc;
  rc = tcsetattr_impl(fd, opt, tio);
  STRACE("tcsetattr(%d, %s, %s) → %d% m", fd, DescribeTcsa(alloca(12), opt),
         DescribeTermios(0, tio), rc);
  return rc;
}
