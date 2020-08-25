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
#include "libc/log/log.h"
#include "libc/runtime/memtrack.h"

STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");

void PrintMemoryIntervals(int fd, const struct MemoryIntervals *mm) {
  int i, frames, maptally, gaptally;
  maptally = 0;
  gaptally = 0;
  (dprintf)(fd, "%s%zd%s\n", "mm->i == ", mm->i, ";");
  for (i = 0; i < mm->i; ++i) {
    if (i && mm->p[i].x != mm->p[i - 1].y + 1) {
      frames = mm->p[i].x - mm->p[i - 1].y - 1;
      gaptally += frames;
      (dprintf)(fd, "%s%,zd%s\n", "/* ", frames, " */");
    }
    frames = mm->p[i].y + 1 - mm->p[i].x;
    maptally += frames;
    (dprintf)(fd, "%s%3u%s0x%08x,0x%08x%s%,zd%s\n", "mm->p[", i, "]=={",
              mm->p[i].x, mm->p[i].y, "}; /* ", frames, " */");
  }
  (dprintf)(fd, "%s%,zd%s%,zd%s\n\n", "/* ", maptally, " frames mapped w/ ",
            gaptally, " frames gapped */");
}
