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
#include "libc/assert.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/calls/termios.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/rop.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#define PTMGET 0x40287401  // openbsd

struct IoctlPtmGet {
  int m;
  int s;
  char mname[16];
  char sname[16];
};

static int openpty_impl(int *mfd, int *sfd, char *name,
                        const struct termios *tio,  //
                        const struct winsize *wsz) {
  int m, s;
  struct IoctlPtmGet t;
  RETURN_ON_ERROR((m = posix_openpt(O_RDWR | O_NOCTTY)));
  if (!IsOpenbsd()) {
    RETURN_ON_ERROR(grantpt(m));
    RETURN_ON_ERROR(unlockpt(m));
    RETURN_ON_ERROR(_ptsname(m, t.sname, sizeof(t.sname)));
    RETURN_ON_ERROR((s = sys_openat(AT_FDCWD, t.sname, O_RDWR, 0)));
  } else {
    RETURN_ON_ERROR(sys_ioctl(m, PTMGET, &t));
    close(m);
    m = t.m;
    s = t.s;
  }
  *mfd = m;
  *sfd = s;
  if (name) strcpy(name, t.sname);
  if (tio) npassert(!tcsetattr(s, TCSAFLUSH, tio));
  if (wsz) npassert(!tcsetwinsize(s, wsz));
  return 0;
OnError:
  if (m != -1) sys_close(m);
  return -1;
}

/**
 * Opens new pseudo teletypewriter.
 *
 * @param mfd receives controlling tty rw fd on success
 * @param sfd receives subordinate tty rw fd on success
 * @param tio may be passed to tune a century of legacy behaviors
 * @param wsz may be passed to set terminal display dimensions
 * @params flags is usually O_RDWR|O_NOCTTY
 * @return 0 on success, or -1 w/ errno
 */
int openpty(int *mfd, int *sfd, char *name,  //
            const struct termios *tio,       //
            const struct winsize *wsz) {
  int rc;
  if (IsWindows() || IsMetal()) {
    return enosys();
  }
  if (IsAsan() && (!__asan_is_valid(mfd, sizeof(int)) ||
                   !__asan_is_valid(sfd, sizeof(int)) ||
                   (name && !__asan_is_valid(name, 16)) ||
                   (tio && !__asan_is_valid(tio, sizeof(*tio))) ||
                   (wsz && !__asan_is_valid(wsz, sizeof(*wsz))))) {
    return efault();
  }
  BLOCK_CANCELATION;
  rc = openpty_impl(mfd, sfd, name, tio, wsz);
  ALLOW_CANCELATION;
  return rc;
}
