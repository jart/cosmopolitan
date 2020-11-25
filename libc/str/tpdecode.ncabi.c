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
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/str/tpdecodecb.internal.h"

forceinline int getbyte(void *arg, uint32_t i) {
  return ((const unsigned char *)arg)[i];
}

/**
 * Thompson-Pike Varint Decoder.
 *
 * @param s is a NUL-terminated string
 * @return number of bytes successfully consumed or -1 w/ errno
 * @note synchronization is performed
 * @see libc/str/tpdecodecb.internal.h (for implementation)
 */
int(tpdecode)(const char *s, wint_t *out) {
  return tpdecodecb(out, (unsigned char)s[0], getbyte, (void *)s);
}
