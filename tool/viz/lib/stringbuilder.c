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
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "tool/viz/lib/stringbuilder.h"

static noinline void StringBuilderGrow(size_t need, struct StringBuilder *sb) {
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
