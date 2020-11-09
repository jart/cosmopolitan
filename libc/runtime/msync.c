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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/sysv/consts/msync.h"

/**
 * Synchronize memory mapping changes to disk.
 *
 * Without this, there's no guarantee memory is written back to disk.
 *
 * @param flags needs MS_ASYNC or MS_SYNC and can have MS_INVALIDATE
 * @return 0 on success or -1 w/ errno
 */
int msync(void *addr, size_t size, int flags) {
  assert(((flags & MS_SYNC) ^ (flags & MS_ASYNC)) || !(MS_SYNC && MS_ASYNC));
  if (!IsWindows()) {
    return msync$sysv(addr, size, flags);
  } else {
    return msync$nt(addr, size, flags);
  }
}
