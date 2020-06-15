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
#include "libc/dce.h"
#include "libc/errno.h"

#define __NR_pipe2_linux 0x0125 /*RHEL5:CVE-2010-3301*/

int32_t pipe2$sysv(int pipefd[hasatleast 2], unsigned flags) {
  int rc, olderr;
  if (!flags) goto OldSkool;
  olderr = errno;
  rc = __pipe2$sysv(pipefd, flags);
  if ((rc == -1 && errno == ENOSYS) ||
      (SupportsLinux() && rc == __NR_pipe2_linux)) {
    errno = olderr;
  OldSkool:
    if ((rc = pipe$sysv(pipefd)) != -1) {
      fixupnewfd$sysv(pipefd[0], flags);
      fixupnewfd$sysv(pipefd[1], flags);
    }
  }
  return rc;
}
