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
#include "libc/bits/pushpop.h"
#include "libc/calls/calls.h"
#include "libc/runtime/mappings.h"
#include "libc/runtime/runtime.h"

/**
 * Closes memory mapping, the Cosmopolitan way.
 *
 * The caller's address holder is set to MAP_FAILED (-1) which is a
 * no-op for subsequent invocations.
 *
 * @return 0 on success, or -1 w/ errno
 */
int munmap_s(void *addrp, uint64_t size) {
  void **addrpp = (void **)addrp;
  void *addr = (void *)pushpop(-1L);
  return munmap(lockxchg(addrpp, &addr), size);
}
