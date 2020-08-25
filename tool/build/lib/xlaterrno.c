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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/carsort.h"
#include "tool/build/lib/xlaterrno.h"

struct LinuxErrno {
  int32_t local;
  int32_t linux;
};

extern const char kLinuxErrnosLength[];
extern const struct LinuxErrno kLinuxErrnos[];
static struct LinuxErrno *errnos;

/**
 * Turns local errno into Linux errno.
 */
int XlatErrno(int local) {
  static bool once;
  long i, n, m, l, r;
  n = (uintptr_t)kLinuxErrnosLength;
  if (!once) {
    errnos = malloc(sizeof(struct LinuxErrno) * n);
    for (i = 0; i < n; ++i) {
      errnos[i].local =
          *(int *)((intptr_t)kLinuxErrnos + kLinuxErrnos[i].local);
      errnos[i].linux = kLinuxErrnos[i].linux;
    }
    carsort100(n, (void *)errnos);
    once = true;
  }
  l = 0;
  r = n - 1;
  while (l <= r) {
    m = (l + r) / 2;
    if (errnos[m].local < local) {
      l = m + 1;
    } else if (errnos[m].local > local) {
      r = m - 1;
    } else {
      return errnos[m].linux;
    }
  }
  return ENOSYS;
}
