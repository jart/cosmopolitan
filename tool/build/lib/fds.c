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
#include "libc/mem/mem.h"
#include "libc/x/x.h"
#include "tool/build/lib/fds.h"

int MachineFdAdd(struct MachineFds *mf) {
  int fd;
  struct MachineFdClosed *closed;
  if ((closed = mf->closed)) {
    DCHECK_LT(closed->fd, mf->i);
    fd = closed->fd;
    mf->closed = closed->next;
    free(closed);
  } else {
    DCHECK_LE(mf->i, mf->n);
    if (mf->i == mf->n) {
      if (!__grow(&mf->p, &mf->n, sizeof(struct MachineFd), 0)) {
        return -1;
      }
    }
    fd = mf->i++;
  }
  return fd;
}

void MachineFdRemove(struct MachineFds *mf, int fd) {
  struct MachineFdClosed *closed;
  DCHECK_GE(fd, 0);
  DCHECK_LT(fd, mf->i);
  mf->p[fd].cb = NULL;
  if ((closed = malloc(sizeof(struct MachineFdClosed)))) {
    closed->fd = fd;
    closed->next = mf->closed;
    mf->closed = closed;
  }
}
