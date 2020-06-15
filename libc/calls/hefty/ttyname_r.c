/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/log/log.h"
#include "libc/nt/console.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static textwindows noinline int ttyname$nt(int fd, char *buf, size_t size) {
  uint32_t mode;
  if (GetConsoleMode(g_fds.p[fd].handle, &mode)) {
    if (mode & kNtEnableVirtualTerminalInput) {
      strncpy(buf, "CONIN$", size);
      return 0;
    } else {
      strncpy(buf, "CONOUT$", size);
      return 0;
    }
  } else {
    return enotty();
  }
}

static int ttyname$freebsd(int fd, char *buf, size_t size) {
  const unsigned FIODGNAME = 2148558456;
  struct fiodgname_arg {
    int len;
    void *buf;
  } fg;
  fg.buf = buf;
  fg.len = size;
  if (ioctl$sysv(fd, FIODGNAME, &fg) != -1) return 0;
  return enotty();
}

static int ttyname$linux(int fd, char *buf, size_t size) {
  struct stat st1, st2;
  if (!isatty(fd)) return errno;
  char name[PATH_MAX];
  snprintf(name, sizeof(name), "/proc/self/fd/%d", fd);
  ssize_t got;
  got = readlink(name, buf, size);
  if (got == -1) return errno;
  if ((size_t)got >= size) return erange();
  buf[got] = 0;
  if (stat(buf, &st1) || fstat(fd, &st2)) return errno;
  if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino) return enodev();
  return 0;
}

int ttyname_r(int fd, char *buf, size_t size) {
  if (IsLinux()) {
    return ttyname$linux(fd, buf, size);
  } else if (IsFreebsd()) {
    return ttyname$freebsd(fd, buf, size);
  } else if (IsWindows()) {
    if (isfdkind(fd, kFdFile)) {
      return ttyname$nt(fd, buf, size);
    } else {
      return ebadf();
    }
  } else {
    return enosys();
  }
}
