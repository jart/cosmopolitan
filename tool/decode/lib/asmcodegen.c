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
#include "tool/decode/lib/asmcodegen.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

char b1[BUFSIZ];
char b2[BUFSIZ];

char *format(char *buf, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  vsnprintf(buf, BUFSIZ, fmt, va);
  va_end(va);
  return buf;
}

char *tabpad(const char *s, unsigned width) {
  char *p;
  size_t i, l, need;
  l = strlen(s);
  need = width > l ? (roundup(width, 8) - l - 1) / 8 + 1 : 0;
  p = memcpy(malloc(l + need + 2), s, l);
  for (i = 0; i < need; ++i) p[l + i] = '\t';
  if (!need) {
    p[l] = ' ';
    ++need;
  }
  p[l + need] = '\0';
  return p;
}

void show(const char *directive, const char *value, const char *comment) {
  if (comment) {
    printf("\t%s\t%s// %s\n", directive, gc(tabpad(value, COLUMN_WIDTH)),
           comment);
  } else {
    printf("\t%s\t%s\n", directive, gc(tabpad(value, COLUMN_WIDTH)));
  }
}
