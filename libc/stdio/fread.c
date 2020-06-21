/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/conv/sizemultiply.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/internal.h"

/**
 * Reads data to stream.
 *
 * @param stride specifies the size of individual items
 * @param count is the number of strides to fetch
 * @return count on success, [0,count) on eof, or 0 on error or count==0
 */
size_t fread(void *buf, size_t stride, size_t count, FILE *f) {
  int c;
  size_t i, bytes;
  unsigned char *p;
  if (!sizemultiply(&bytes, stride, count)) {
    return fseterr(f, EOVERFLOW);
  }
  for (p = buf, i = 0; i < bytes; ++i) {
    if ((c = fgetc(f)) == -1) {
      if (i % stride != 0) abort(); /* todo(jart) */
      return i / stride;
    }
    p[i] = c & 0xff;
  }
  return count;
}
