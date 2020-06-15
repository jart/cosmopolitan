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
#include "libc/dce.h"
#include "libc/sysv/errfuns.h"

/**
 * Sends signal to process.
 *
 * The impact of this action can be terminating the process, or
 * interrupting it to request something happen.
 *
 * @param pid can be:
 *      >0 signals one process by id
 *      =0 signals all processes in current process group
 *      -1 signals all processes possible (except init)
 *     <-1 signals all processes in -pid process group
 * @param sig can be:
 *      >0 can be SIGINT, SIGTERM, SIGKILL, SIGUSR1, etc.
 *      =0 is for error checking
 * @return 0 if something was accomplished, or -1 w/ errno
 * @asyncsignalsafe
 */
int kill(int pid, int sig) {
  if (pid == getpid()) return raise(sig);
  if (!IsWindows()) {
    return kill$sysv(pid, sig, 1);
  } else {
    return enosys();
  }
}
