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
#include "libc/mem/mem.h"

/**
 * Executes program, with path environment search.
 *
 * The current process is replaced with the executed one.
 *
 * @param prog is the program to launch
 * @param argv is [file,argv₁..argvₙ₋₁,NULL]
 * @param envp is ["key=val",...,NULL]
 * @return doesn't return on success, otherwise -1 w/ errno
 * @notasyncsignalsafe
 */
int execvpe(const char *prog, char *const argv[], char *const *envp) {
  char *exe;
  char pathbuf[PATH_MAX];
  if ((exe = commandv(prog, pathbuf))) {
    execve(exe, argv, envp);
  }
  return -1;
}
