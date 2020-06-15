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
#include "libc/conv/conv.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

float b32;
double b64;
long double b80;
uint32_t u32;
uint64_t u64;
intmax_t x;

void int2float(const char *s) {
  x = strtoimax(s, NULL, 0);
  if ((0 <= x && x <= UINT32_MAX) && !startswith(s, "-") &&
      (!endswith(s, "l") && !endswith(s, "L"))) {
    u32 = x;
    memcpy(&b32, &u32, 4);
    s = gc(xdtoa(b32));
    if (!strchr(s, '.')) s = gc(xasprintf("%s.", s));
    s = gc(xasprintf("%sf", s));
    puts(s);
  } else if ((0 <= x && x <= UINT64_MAX) && !startswith(s, "-")) {
    u64 = x;
    memcpy(&b64, &u64, 8);
    s = gc(xdtoa(b64));
    if (!strchr(s, '.')) s = gc(xasprintf("%s.", s));
    puts(s);
  } else if ((INT32_MIN <= x && x <= 0) &&
             (!endswith(s, "l") && !endswith(s, "L"))) {
    u32 = ABS(x);
    memcpy(&b32, &u32, 4);
    b32 = -b32;
    s = gc(xdtoa(b32));
    if (!strchr(s, '.')) s = gc(xasprintf("%s.", s));
    s = gc(xasprintf("%sf", s));
    puts(s);
  } else if (INT64_MIN <= x && x <= 0) {
    u64 = ABS(x);
    memcpy(&b64, &u64, 8);
    b64 = -b64;
    s = gc(xdtoa(b64));
    if (!strchr(s, '.')) s = gc(xasprintf("%s.", s));
    puts(s);
  } else {
    memcpy(&b80, &x, 16);
    s = gc(xdtoa(b80));
    if (!strchr(s, '.')) s = gc(xasprintf("%s.", s));
    s = gc(xasprintf("%sL", s));
    puts(s);
  }
}

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    int2float(argv[i]);
  }
  return 0;
}
