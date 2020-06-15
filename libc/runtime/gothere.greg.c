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
#include "libc/conv/itoa.h"
#include "libc/runtime/missioncritical.h"

/**
 * Debugs super low-level stuff, e.g.
 *
 *   void GOT_HERE(int);
 *   INITIALIZER(100, _init_got_here_100, GOT_HERE(100));
 *   INITIALIZER(200, _init_got_here_200, GOT_HERE(200));
 *   INITIALIZER(300, _init_got_here_300, GOT_HERE(300));
 *   INITIALIZER(400, _init_got_here_400, GOT_HERE(400));
 *   INITIALIZER(500, _init_got_here_500, GOT_HERE(500));
 *   INITIALIZER(600, _init_got_here_600, GOT_HERE(600));
 *   INITIALIZER(700, _init_got_here_700, GOT_HERE(700));
 *   INITIALIZER(800, _init_got_here_800, GOT_HERE(800));
 *
 */
privileged interruptfn void GOT_HERE(long num) {
  size_t len;
  char msg[48];
  len = 0;
  msg[len++] = 'g';
  msg[len++] = 'o';
  msg[len++] = 't';
  msg[len++] = ' ';
  msg[len++] = 'h';
  msg[len++] = 'e';
  msg[len++] = 'r';
  msg[len++] = 'e';
  msg[len++] = ' ';
  len += int64toarray_radix10(num, &msg[len]);
  msg[len++] = '\n';
  msg[len] = '\0';
  __print(msg, len);
}
