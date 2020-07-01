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
#include "libc/fmt/fmt.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"

STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");

static void onmemchunk(void *start, void *end, size_t used_bytes, void *arg) {
  FILE *f = arg;
  (fprintf)(f, "%p - %p : %08zx / %08lx\n", start, end, used_bytes,
            (intptr_t)end - (intptr_t)start);
}

/**
 * Prints memory mappings.
 */
void meminfo(FILE *f) {
  memsummary(f);
  (fprintf)(f, "%*s   %*s   %*s   %*s\n", POINTER_XDIGITS, "start",
            POINTER_XDIGITS, "end", 8, "used", 8, "size");
  malloc_inspect_all(onmemchunk, f);
}
