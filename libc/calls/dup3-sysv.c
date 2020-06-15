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
#include "libc/calls/internal.h"
#include "libc/errno.h"

#define __NR_dup3_linux 0x0124 /*RHEL5:CVE-2010-3301*/

int32_t dup3$sysv(int32_t oldfd, int32_t newfd, int flags) {
  static bool once, demodernize;
  int olderr, fd;
  if (!once) {
    once = true;
    olderr = errno;
    fd = __dup3$sysv(oldfd, newfd, flags);
    if ((fd == -1 && errno == ENOSYS) || fd == __NR_dup3_linux) {
      demodernize = true;
      errno = olderr;
      goto OldSkool;
    }
  } else if (demodernize) {
    goto OldSkool;
  } else {
    fd = __dup3$sysv(oldfd, newfd, flags);
  }
  return fd;
OldSkool:
  fd = dup2$sysv(oldfd, newfd);
  if (flags) fd = fixupnewfd$sysv(fd, flags);
  return fd;
}
