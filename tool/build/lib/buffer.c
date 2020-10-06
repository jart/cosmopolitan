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
#include "libc/alg/arraylist2.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/str/tpencode.h"
#include "tool/build/lib/buffer.h"

void AppendChar(struct Buffer *b, char c) {
  APPEND(&b->p, &b->i, &b->n, &c);
}

void AppendData(struct Buffer *b, char *data, size_t len) {
  CONCAT(&b->p, &b->i, &b->n, data, len);
}

void AppendStr(struct Buffer *b, const char *s) {
  AppendData(b, s, strlen(s));
}

void AppendWide(struct Buffer *b, wint_t wc) {
  char cbuf[8];
  AppendData(b, cbuf, tpencode(cbuf, 8, wc, false));
}

void AppendFmt(struct Buffer *b, const char *fmt, ...) {
  int size;
  char *tmp;
  va_list va;
  tmp = NULL;
  va_start(va, fmt);
  size = vasprintf(&tmp, fmt, va);
  va_end(va);
  if (size != -1) AppendData(b, tmp, size);
  free(tmp);
}

/**
 * Writes buffer until completion, interrupt, or error occurs.
 */
ssize_t WriteBuffer(struct Buffer *b, int fd) {
  char *p;
  ssize_t rc;
  size_t wrote, n;
  p = b->p;
  n = b->i;
  do {
    if ((rc = write(fd, p, n)) != -1) {
      wrote = rc;
      p += wrote;
      n -= wrote;
    } else if (errno == EINTR) {
      break;
    } else {
      return -1;
    }
  } while (n);
  return 0;
}
