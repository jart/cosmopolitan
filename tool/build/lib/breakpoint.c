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
#include "libc/alg/arraylist2.internal.h"
#include "libc/assert.h"
#include "libc/log/log.h"
#include "tool/build/lib/breakpoint.h"

void PopBreakpoint(struct Breakpoints *bps) {
  if (bps->i) {
    --bps->i;
  }
}

ssize_t PushBreakpoint(struct Breakpoints *bps, struct Breakpoint *b) {
  int i;
  for (i = 0; i < bps->i; ++i) {
    if (bps->p[i].disable) {
      memcpy(&bps->p[i], b, sizeof(*b));
      return i;
    }
  }
  return APPEND(&bps->p, &bps->i, &bps->n, b);
}

ssize_t IsAtBreakpoint(struct Breakpoints *bps, int64_t addr) {
  size_t i;
  for (i = bps->i; i--;) {
    if (bps->p[i].disable) continue;
    if (bps->p[i].addr == addr) {
      if (bps->p[i].oneshot) {
        bps->p[i].disable = true;
        if (i == bps->i - 1) {
          --bps->i;
        }
      }
      return i;
    }
  }
  return -1;
}
