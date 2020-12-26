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
#include "libc/bits/pushpop.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

STATIC_YOINK("_init_g_stdout");

/**
 * Pointer to standard output stream.
 */
FILE *stdout;

hidden FILE g_stdout;
hidden unsigned char g_stdout_buf[BUFSIZ] forcealign(PAGESIZE);

static textstartup void _init_g_stdout2() {
  struct FILE *sf;
  sf = stdout;
  asm("" : "+r"(sf));
  if (IsWindows() || ischardev(pushpop(sf->fd))) {
    sf->bufmode = _IOLBF;
  }
  _fflushregister(sf);
}

const void *const g_stdout_ctor[] initarray = {_init_g_stdout2};
