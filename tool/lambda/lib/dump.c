/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/kprintf.h"
#include "tool/lambda/lib/blc.h"

void Dumper(int i, int j, FILE *f) {
  char buf[64];
  if (i) fputc('\n', f);
  for (; i < j; ++i) {
    switch (mem[i]) {
      case VAR:
        ksnprintf(buf, sizeof(buf), "    %s,%d,\t// %2d: ", "VAR", mem[i + 1],
                  i);
        fputs(buf, f);
        Print(i, 1, 0, f);
        fputc('\n', f);
        ++i;
        break;
      case APP:
        ksnprintf(buf, sizeof(buf), "    %s,%d,\t// %2d: ", "APP", mem[i + 1],
                  i);
        fputs(buf, f);
        Print(i, 1, 0, f);
        fputc('\n', f);
        ++i;
        break;
      case ABS:
        ksnprintf(buf, sizeof(buf), "    %s,\t// %2d: ", "ABS", i);
        fputs(buf, f);
        Print(i, 1, 0, f);
        fputc('\n', f);
        break;
      default:
        ksnprintf(buf, sizeof(buf), "    %d,\t// %2d: ", mem[i], i);
        fputs(buf, f);
        Print(i, 1, 0, f);
        fputc('\n', f);
        break;
    }
  }
}

void Dump(int i, int j, FILE *f) {
  fputs("\nstatic int kTerm[] = {\n", f);
  Dumper(i, j, f);
  fputs("};\n", f);
}
