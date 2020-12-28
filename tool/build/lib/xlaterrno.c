/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
