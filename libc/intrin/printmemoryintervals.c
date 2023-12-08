/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"

static bool IsNoteworthyHole(unsigned i, const struct MemoryIntervals *mm) {
  // gaps between shadow frames aren't interesting
  // the chasm from heap to stack ruins statistics
  return !(
      (IsShadowFrame(mm->p[i].y) || IsShadowFrame(mm->p[i + 1].x)) ||
      (!IsStaticStackFrame(mm->p[i].y) && IsStaticStackFrame(mm->p[i + 1].x)));
}

void PrintMemoryIntervals(int fd, const struct MemoryIntervals *mm) {
  long i, w, frames, maptally = 0;
  char mappingbuf[8], framebuf[64], sb[16];
  for (w = i = 0; i < mm->i; ++i) {
    w = MAX(w, LengthInt64Thousands(mm->p[i].y + 1 - mm->p[i].x));
  }
  for (i = 0; i < mm->i; ++i) {
    frames = mm->p[i].y + 1 - mm->p[i].x;
    maptally += frames;
    kprintf("%08x-%08x %s %'*ldx %s", mm->p[i].x, mm->p[i].y,
            (DescribeMapping)(mappingbuf, mm->p[i].prot, mm->p[i].flags), w,
            frames, (DescribeFrame)(framebuf, mm->p[i].x));
    if (mm->p[i].iscow) kprintf(" cow");
    if (mm->p[i].readonlyfile) kprintf(" readonlyfile");
    sizefmt(sb, mm->p[i].size, 1024);
    kprintf(" %sB", sb);
    if (i + 1 < mm->i) {
      frames = mm->p[i + 1].x - mm->p[i].y - 1;
      if (frames && IsNoteworthyHole(i, mm)) {
        sizefmt(sb, frames * FRAMESIZE, 1024);
        kprintf(" w/ %sB hole", sb);
      }
    }
    if (mm->p[i].h != -1) {
      kprintf(" h=%ld", mm->p[i].h);
    }
    kprintf("\n");
  }
  sizefmt(sb, maptally * FRAMESIZE, 1024);
  kprintf("# %sB total mapped memory\n", sb);
}
