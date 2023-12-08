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
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "tool/viz/lib/stringbuilder.h"

static dontinline void StringBuilderGrow(size_t need, struct StringBuilder *sb) {
  size_t n2;
  n2 = MAX(16, sb->n);
  while (sb->i + need > n2) n2 += n2 >> 1;
  sb->p = xrealloc(sb->p, n2);
  sb->n = n2;
}

struct StringBuilder *NewStringBuilder(void) {
  return xcalloc(1, sizeof(struct StringBuilder));
}

int StringBuilderAppend(const char *s, struct StringBuilder *sb) {
  size_t size;
  CHECK_LE(sb->i, sb->n);
  size = strlen(s);
  if (sb->i + size + 1 > sb->n) StringBuilderGrow(size + 1, sb);
  memcpy(sb->p + sb->i, s, size + 1);
  sb->i += size;
  return 0;
}

char *FreeStringBuilder(struct StringBuilder *sb) {
  char *res;
  CHECK_LE(sb->i, sb->n);
  CHECK_EQ('\0', sb->p[sb->i]);
  res = xrealloc(sb->p, sb->i + 1);
  free(sb);
  return res;
}
