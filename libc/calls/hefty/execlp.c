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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/calls/hefty/mkvarargv.h"
#include "libc/calls/calls.h"

/**
 * Executes program, with PATH search and current environment.
 *
 * The current process is replaced with the executed one.
 *
 * @param prog is program to launch (may be PATH searched)
 * @param arg[0] is the name of the program to run
 * @param arg[1,n-2] optionally specify program arguments
 * @param arg[n-1] is NULL
 * @return doesn't return on success, otherwise -1 w/ errno
 * @notasyncsignalsafe
 */
int execlp(const char *prog, const char *arg, ... /*, NULL*/) {
  char *exe;
  if ((exe = commandv(prog))) {
    va_list va;
    void *argv;
    va_start(va, arg);
    if ((argv = mkvarargv(arg, va))) {
      execve(exe, argv, environ);
      free(argv);
    }
    va_end(va);
  }
  return -1;
}
