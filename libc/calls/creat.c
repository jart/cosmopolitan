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
#include "libc/sysv/consts/o.h"

/**
 * Creates new file, returning open()'d file descriptor.
 *
 * @param file is a UTF-8 string, which is truncated if it exists
 * @param mode is an octal user/group/other permission, e.g. 0755
 * @return a number registered with the system to track the open file,
 *     which must be stored using a 64-bit type in order to support both
 *     System V and Windows, and must be closed later on using close()
 * @see touch()
 * @asyncsignalsafe
 */
nodiscard int creat(const char *file, uint32_t mode) {
  return open(file, O_CREAT | O_WRONLY | O_TRUNC, mode);
}
