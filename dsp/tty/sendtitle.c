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
#include "dsp/tty/tty.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/runtime/gc.h"
#include "libc/x/x.h"

/* TODO(jart): DELETE */

/**
 * Changes text in title bar of pseudo-teletypewriter window.
 *
 * @param title is trustworthy text without any BEL characters
 * @param ti comes from ttyident() and null means no-op
 */
int ttysendtitle(int ttyfd, const char *title, const struct TtyIdent *ti) {
  if (ti) {
    if (ti->id == kTtyIdScreen) {
      return ttysend(ttyfd, gc(xstrcat("\eP\e]0;", title, "\a\e\\")));
    } else {
      return ttysend(ttyfd, gc(xstrcat("\e]0;", title, "\a")));
    }
  } else {
    return 0;
  }
}
