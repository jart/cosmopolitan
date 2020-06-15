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
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

testonly bool testlib_strcaseequals(size_t cw, const void *s1, const void *s2) {
  return testlib_strncaseequals(cw, s1, s2, SIZE_MAX);
}

testonly bool testlib_strncaseequals(size_t cw, const void *s1, const void *s2,
                                     size_t n) {
  if (s1 == s2) return true;
  if (!s1 || !s2) return false;
  return (cw == sizeof(wchar_t)
              ? wcsncasecmp(s1, s2, n)
              : cw == sizeof(char16_t) ? strncasecmp16(s1, s2, n)
                                       : strncasecmp(s1, s2, n)) == 0;
}
