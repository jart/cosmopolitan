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
#include "libc/bits/safemacros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/calls/calls.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns current working directory.
 *
 * If the PWD environment variable is set, that'll be returned (since
 * it's faster than issuing a system call).
 *
 * @return pointer that must be free()'d, or NULL w/ errno
 */
nodiscard char *get_current_dir_name(void) {
  char *buf, *res;
  if ((res = getenv("PWD"))) return strdup(res);
  if (!(buf = malloc(PATH_MAX))) return NULL;
  if (!(res = (getcwd)(buf, PATH_MAX))) free(buf);
  return res;
}
