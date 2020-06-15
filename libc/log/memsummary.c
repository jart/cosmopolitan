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
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"

STATIC_YOINK("ntoa");

void memsummary(FILE *f) {
  struct mallinfo mi = mallinfo();
  (fprintf)(f,
            "arena\t\t%,-12zu\t# space allocated from system\n"
            "ordblks\t\t%,-12zu\t# number of free chunks\n"
            "hblkhd\t\t%,-12zu\t# space in mmapped regions\n"
            "usmblks\t\t%,-12zu\t# maximum total allocated space\n"
            "uordblks\t%,-12zu\t# total allocated space\n"
            "fordblks\t%,-12zu\t# total free space\n"
            "keepcost\t%,-12zu\t# releasable (via malloc_trim) space\n\n",
            mi.arena, mi.ordblks, mi.hblkhd, mi.usmblks, mi.uordblks,
            mi.fordblks, mi.keepcost);
}
