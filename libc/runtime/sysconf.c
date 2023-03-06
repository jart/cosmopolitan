/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/rlimit.h"

/**
 * Returns configuration value about system.
 *
 * The following parameters are supported:
 *
 * - `_SC_CLK_TCK` returns number of clock ticks per second
 * - `_SC_ARG_MAX` currently always returns 32768 due to Windows
 * - `_SC_PAGESIZE` currently always returns 65536 due to Windows
 * - `_SC_NPROCESSORS_ONLN` returns number of CPUs in the system
 * - `_SC_OPEN_MAX` returns maximum number of open files
 * - `_SC_CHILD_MAX` returns maximum number of processes
 *
 */
long sysconf(int name) {
  int n;
  switch (name) {
    case _SC_ARG_MAX:
      return _ARG_MAX;
    case _SC_CHILD_MAX:
      return _GetResourceLimit(RLIMIT_NPROC);
    case _SC_CLK_TCK:
      return CLK_TCK;
    case _SC_OPEN_MAX:
      return _GetMaxFd();
    case _SC_PAGESIZE:
      return FRAMESIZE;
    case _SC_NPROCESSORS_ONLN:
      n = _getcpucount();
      return n > 0 ? n : -1;
    default:
      return -1;
  }
}
