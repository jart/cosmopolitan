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
#include "libc/calls/hefty/internal.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"

/**
 * Replaces current process with program.
 *
 * @param program will not be PATH searched, see commandv()
 * @param argv[0] is the name of the program to run
 * @param argv[1,n-2] optionally specify program arguments
 * @param argv[n-1] is NULL
 * @param envp[0,n-2] specifies "foo=bar" environment variables
 * @param envp[n-1] is NULL
 * @return doesn't return, or -1 w/ errno
 * @asyncsignalsafe
 */
int execve(const char *program, char *const argv[], char *const envp[]) {
  if (!IsWindows()) {
    return execve$sysv(program, argv, envp);
  } else {
    return execve$nt(program, argv, envp);
  }
}
