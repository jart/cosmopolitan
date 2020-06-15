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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

/**
 * Closes standard i/o stream and its underlying thing.
 *
 * @param f is the file object, which is always free if it's heap,
 *     otherwise its resources are released and fields updated
 * @return 0 on success or -1 on error, which can be a trick for
 *     differentiating between EOF and real errors during previous
 *     i/o calls, without needing to call ferror()
 * @see fclose_s()
 */
int fclose(FILE *f) {
  int rc;
  if (!f) return 0; /* good java behavior; glibc crashes */
  fflushunregister(f);
  fflush(f);
  free_s(&f->buf);
  f->state = EOF;
  if (f->noclose) {
    f->fd = -1;
  } else if (close_s(&f->fd) == -1) {
    f->state = errno;
  }
  if (f->state == EOF) {
    rc = 0;
  } else {
    errno = f->state;
    rc = EOF;
  }
  free_s(&f);
  return rc;
}
