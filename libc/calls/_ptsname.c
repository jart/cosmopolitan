/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#define TIOCGPTN     0x80045430  // linux
#define TIOCPTYGNAME 0x40807453  // xnu
#define TIOCPTSNAME  0x48087448  // netbsd
#define FIODGNAME    0x80106678  // freebsd

struct fiodgname_arg {
  int len;
  void *buf;
};

struct ptmget {
  int cfd;
  int sfd;
  char cn[1024];
  char sn[1024];
};

int _ptsname(int fd, char *buf, size_t size) {
  int pty;
  size_t n;
  struct ptmget t;

  if (_isptmaster(fd)) {
    return -1;
  }

  t.sn[0] = '/';
  t.sn[1] = 'd';
  t.sn[2] = 'e';
  t.sn[3] = 'v';
  t.sn[4] = '/';
  t.sn[5] = 0;

  if (IsLinux()) {
    if (sys_ioctl(fd, TIOCGPTN, &pty)) return -1;
    t.sn[5] = 'p';
    t.sn[6] = 't';
    t.sn[7] = 's';
    t.sn[8] = '/';
    FormatInt32(t.sn + 9, pty);
  } else if (IsXnu()) {
    if (sys_ioctl(fd, TIOCPTYGNAME, t.sn)) {
      return -1;
    }
  } else if (IsFreebsd()) {
    struct fiodgname_arg fgn = {sizeof(t.sn) - 5, t.sn + 5};
    if (sys_ioctl(fd, FIODGNAME, &fgn) == -1) {
      if (errno == EINVAL) {
        errno = ERANGE;
      }
      return -1;
    }
  } else if (IsNetbsd()) {
    if (sys_ioctl(fd, TIOCPTSNAME, &t)) {
      return -1;
    }
  } else {
    return enosys();
  }

  if ((n = strlen(t.sn)) < size) {
    memcpy(buf, t.sn, n + 1);
    return 0;
  } else {
    return erange();
  }
}
