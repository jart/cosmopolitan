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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/log.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define FIODGNAME 0x80106678  // freebsd

static textwindows errno_t sys_ttyname_nt(int fd, char *buf, size_t size) {
  if (fd + 0u >= g_fds.n) return EBADF;
  if (g_fds.p[fd].kind != kFdConsole) return ENOTTY;
  if (strlcpy(buf, "/dev/tty", size) >= size) return ERANGE;
  return 0;
}

// clobbers errno
static errno_t ttyname_freebsd(int fd, char *buf, size_t size) {
  struct fiodgname_arg {
    int len;
    void *buf;
  } fg;
  fg.buf = buf;
  fg.len = size;
  if (sys_ioctl(fd, FIODGNAME, &fg) != -1) {
    return 0;
  } else {
    return ENOTTY;
  }
}

// clobbers errno
static errno_t ttyname_linux(int fd, char *buf, size_t size) {
  ssize_t got;
  struct stat st1, st2;
  char name[14 + 12 + 1];
  if (!isatty(fd)) return errno;
  FormatInt32(stpcpy(name, "/proc/self/fd/"), fd);
  got = readlink(name, buf, size);
  if (got == -1) return errno;
  if (got >= size) return ERANGE;
  buf[got] = 0;
  if (stat(buf, &st1) || fstat(fd, &st2)) return errno;
  if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino) return ENODEV;
  return 0;
}

/**
 * Returns name of terminal.
 *
 * @return 0 on success, or error number on error
 * @raise ERANGE if `size` was too small
 * @returnserrno
 */
errno_t ttyname_r(int fd, char *buf, size_t size) {
  errno_t e, res;
  e = errno;
  if (IsLinux()) {
    res = ttyname_linux(fd, buf, size);
  } else if (IsFreebsd()) {
    res = ttyname_freebsd(fd, buf, size);
  } else if (IsWindows()) {
    res = sys_ttyname_nt(fd, buf, size);
  } else {
    // TODO(jart): Use that fstat(dev/ino) + readdir(/dev/) trick.
    if (strlcpy(buf, "/dev/tty", size) < size) {
      res = 0;
    } else {
      res = ERANGE;
    }
  }
  errno = e;
  STRACE("ttyname_r(%d, %#.*hhs) → %s", fd, (int)strnlen(buf, size), buf,
         !res ? "0" : _strerrno(res));
  return res;
}
