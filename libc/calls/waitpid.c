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

/**
 * Waits for status to change on process.
 *
 * @param pid >0 targets specific process, =0 means any proc in a group,
 *     -1 means any child process, <-1 means any proc in specific group
 * @param opt_out_wstatus optionally returns status code, and *wstatus
 *     may be inspected using WEXITSTATUS(), etc.
 * @param options can have WNOHANG, WUNTRACED, WCONTINUED, etc.
 * @return process id of terminated child or -1 w/ errno
 * @asyncsignalsafe
 */
int waitpid(int pid, int *opt_out_wstatus, int options) {
  if (!IsWindows()) {
    return wait4$sysv(pid, opt_out_wstatus, options, NULL);
  } else {
    return wait4$nt(pid, opt_out_wstatus, options, NULL);
  }
}
