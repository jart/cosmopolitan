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
#include "libc/calls/internal.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

void __on_ioctl_tcsets(int);
int ioctl_tcsets_nt(int, uint64_t, const struct termios *);

static int ioctl_tcsets_metal(int fd, uint64_t request,
                              const struct termios *tio) {
  return 0;
}

static int ioctl_tcsets_sysv(int fd, uint64_t request,
                             const struct termios *tio) {
  union metatermios mt;
  if (IsAsan() && !__asan_is_valid(tio, sizeof(*tio))) return efault();
  return sys_ioctl(fd, request, __termios2host(&mt, tio));
}

/**
 * Changes terminal behavior.
 *
 * @see tcsetattr(fd, TCSA{NOW,DRAIN,FLUSH}, tio) dispatches here
 * @see ioctl(fd, TCSETS{,W,F}, tio) dispatches here
 * @see ioctl(fd, TIOCGETA{,W,F}, tio) dispatches here
 */
int ioctl_tcsets(int fd, uint64_t request, ...) {
  int rc;
  va_list va;
  static bool once;
  const struct termios *tio;
  va_start(va, request);
  tio = va_arg(va, const struct termios *);
  va_end(va);
  if (0 <= fd && fd <= 2 && _weaken(__on_ioctl_tcsets)) {
    if (!once) {
      _weaken(__on_ioctl_tcsets)(fd);
      once = true;
    }
  }
  if (!tio || (IsAsan() && !__asan_is_valid(tio, sizeof(*tio)))) {
    rc = efault();
  } else if (fd >= 0) {
    if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
      rc = enotty();
    } else if (IsMetal()) {
      rc = ioctl_tcsets_metal(fd, request, tio);
    } else if (!IsWindows()) {
      rc = ioctl_tcsets_sysv(fd, request, tio);
    } else {
      rc = ioctl_tcsets_nt(fd, request, tio);
    }
  } else {
    rc = einval();
  }
  STRACE("ioctl_tcsets(%d, %p, %p) → %d% m", fd, request, tio, rc);
  return rc;
}
