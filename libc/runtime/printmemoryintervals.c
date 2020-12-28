/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
  (dprintf)(fd, "%s%zd%s\r\n", "mm->i == ", mm->i, ";");
  for (i = 0; i < mm->i; ++i) {
    if (i && mm->p[i].x != mm->p[i - 1].y + 1) {
      frames = mm->p[i].x - mm->p[i - 1].y - 1;
      gaptally += frames;
      (dprintf)(fd, "%s%,zd%s\r\n", "/* ", frames, " */");
    }
    frames = mm->p[i].y + 1 - mm->p[i].x;
    maptally += frames;
    (dprintf)(fd, "%s%3u%s0x%08x,0x%08x%s%,zd%s\r\n", "mm->p[", i, "]=={",
              mm->p[i].x, mm->p[i].y, "}; /* ", frames, " */");
  }
  (dprintf)(fd, "%s%,zd%s%,zd%s\r\n\r\n", "/* ", maptally, " frames mapped w/ ",
            gaptally, " frames gapped */");
}
