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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/dce.h"
#include "libc/nt/accounting.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/memorystatusex.h"
#include "libc/nt/systeminfo.h"
#include "libc/str/str.h"

/**
 * Returns amount of system ram, cores, etc.
 * @return 0 on success or -1 w/ errno
 * @error ENOSYS, EFAULT
 */
int sysinfo(struct sysinfo *info) {
  int rc;
  memset(info, 0, sizeof(*info));
  if (!IsWindows()) {
    rc = sysinfo$sysv(info);
  } else {
    rc = sysinfo$nt(info);
  }
  info->procs = max(1, info->procs);
  info->mem_unit = max(1, info->mem_unit);
  info->totalram = max((8 * 1024 * 1024) / info->mem_unit, info->totalram);
  return rc;
}
