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
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/macros.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

static struct AtFork {
  size_t i;
  struct AtForkCallback {
    void (*fn)(void *);
    void *arg;
  } p[ATEXIT_MAX];
} g_atfork;

/**
 * Registers function to be called when PID changes.
 *
 * @return 0 on success, or -1 w/ errno
 */
int atfork(void *fn, void *arg) {
  if (g_atfork.i == ARRAYLEN(g_atfork.p)) return enomem();
  g_atfork.p[g_atfork.i++] = (struct AtForkCallback){.fn = fn, .arg = arg};
  return 0;
}

/**
 * Triggers atfork() callbacks.
 *
 * Only fork() should call this.
 */
void __onfork(void) {
  size_t i;
  for (i = 0; i < g_atfork.i; ++i) {
    g_atfork.p[i].fn(g_atfork.p[i].arg);
  }
}
