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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/mem/mem.h"
#include "libc/sysv/errfuns.h"

int growfds(void) {
  size_t i, n;
  struct Fd *p;
  if (weaken(realloc)) {
    if ((p = weaken(realloc)(g_fds.p != g_fds.__init_p ? g_fds.p : NULL,
                             (n = ((i = g_fds.n) * 2)) * sizeof(*p)))) {
      do {
        p[i++].kind = kFdEmpty;
      } while (i < n);
      g_fds.p = p;
      g_fds.n = n;
      return 0;
    } else {
      return enomem();
    }
  } else {
    return emfile();
  }
}
