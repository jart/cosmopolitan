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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/dce.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/**
 * Launches program with system command interpreter.
 *
 * @param cmdline is an interpreted Turing-complete command
 * @return -1 if child process couldn't be created, otherwise a wait
 *     status that can be accessed using macros like WEXITSTATUS(s)
 */
int system(const char *cmdline) {
  const char *prog, *arg;
  int rc, wstatus, fds[3];
  if (weaken(fflush)) weaken(fflush)(NULL);
  if (cmdline) {
    prog = !IsWindows() ? _PATH_BSHELL : "cmd";
    arg = !IsWindows() ? "-c" : "/C";
    fds[0] = 0;
    fds[1] = 1;
    fds[2] = 2;
    if ((rc = spawnlp(0, fds, prog, prog, arg, cmdline, NULL)) != -1) {
      if ((rc = wait4(rc, &wstatus, 0, NULL)) != -1) {
        rc = wstatus;
      }
    }
    return rc;
  } else if (IsWindows()) {
    return true;
  } else {
    return fileexists(_PATH_BSHELL);
  }
}
