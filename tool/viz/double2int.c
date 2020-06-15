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
#include "third_party/dtoa/dtoa.h"

void double2int(const char *s) {
  double b64;
  uint64_t u64;
  b64 = strtod(s, NULL);
  memcpy(&u64, &b64, 8);
  printf("0x%016lx\n", u64);
}

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    double2int(argv[i]);
  }
  return 0;
}
