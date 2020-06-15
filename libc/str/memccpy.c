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
#include "libc/str/str.h"

/**
 * Copies at most 𝑛 bytes from 𝑠 to 𝑑 until 𝑐 is encountered.
 *
 * This is little-known C Standard Library approach, dating back to the
 * Fourth Edition of System Five, for copying a C strings to fixed-width
 * buffers, with added generality.
 *
 * For example, strictly:
 *
 *   char buf[16];
 *   CHECK_NOTNULL(memccpy(buf, s, '\0', sizeof(buf)));
 *
 * Or unstrictly:
 *
 *   if (!memccpy(buf, s, '\0', sizeof(buf))) strcpy(buf, "?");
 *
 * Are usually more sensible than the following:
 *
 *   char cstrbuf[16];
 *   snprintf(cstrbuf, sizeof(cstrbuf), "%s", CSTR);
 *
 * @return 𝑑 + idx(𝑐) + 1, or NULL if 𝑐 ∉ 𝑠₀․․ₙ₋₁
 * @note 𝑑 and 𝑠 can't overlap
 * @asyncsignalsafe
 */
void *memccpy(void *d, const void *s, int c, size_t n) {
  const char *p, *pe;
  if ((pe = memchr((p = s), c, n))) {
    return mempcpy(d, s, pe - p + 1);
  } else {
    memcpy(d, s, n);
    return NULL;
  }
}
