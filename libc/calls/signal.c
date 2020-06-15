/*-*- mode:c; indent-tabs-mode:nil; tab-width:2; coding:utf-8               -*-│
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/sysv/consts/sa.h"

/**
 * Installs kernel interrupt handler.
 *
 * @see sigaction() which has more features
 */
sighandler_t(signal)(int sig, sighandler_t func) {
  struct sigaction sa_old, sa = {.sa_handler = func, .sa_flags = SA_RESTART};
  if ((sigaction)(sig, &sa, &sa_old) == -1) return SIG_ERR;
  return sa_old.sa_handler;
}
