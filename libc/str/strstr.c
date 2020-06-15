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
#include "libc/alg/alg.h"
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"

/**
 * Searches for substring.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 * @see memmem()
 */
char *(strstr)(const char *haystack, const char *needle) {
  if (needle[0]) {
    if (needle[1]) {
      if (!((intptr_t)needle & 0xf) && X86_HAVE(SSE4_2) && !IsTiny()) {
        return strstr$sse42(haystack, needle);
      } else {
        size_t needlelen;
        alignas(16) char needle2[64];
        needlelen = strlen(needle);
        if (needlelen < 64 && X86_HAVE(SSE4_2) && !IsTiny()) {
          memcpy(needle2, needle, (needlelen + 1) * sizeof(char));
          return strstr$sse42(haystack, needle2);
        } else {
          return tinystrstr(haystack, needle);
        }
      }
    } else {
      return strchr(haystack, needle[0]);
    }
  } else {
    return haystack;
  }
}
