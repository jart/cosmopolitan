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
#include "libc/bits/popcnt.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/errfuns.h"

/**
 * Tunes buffering settings for an stdio stream.
 *
 * @param mode may be _IOFBF, _IOLBF, or _IONBF
 * @param buf may optionally be non-NULL to set the stream's underlying
 *     buffer, which the stream will own, but won't free
 * @param size must be a two power if buf is provided
 * @return 0 on success or -1 on error
 */
int setvbuf(FILE *f, char *buf, int mode, size_t size) {
  if (size && popcnt(size) != 1) return einval();
  setbuffer(f, buf, size);
  f->bufmode = mode;
  return 0;
}
