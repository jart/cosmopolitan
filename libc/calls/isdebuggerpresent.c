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
#include "libc/alg/alg.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"

#define kBufSize    1024
#define kProcStatus "/proc/self/status"
alignas(16) static const char kGdbPid[] = "TracerPid:\t";

/**
 * Determines if gdb, strace, windbg, etc. is controlling process.
 * @return non-zero if attached, otherwise 0
 */
int IsDebuggerPresent(bool force) {
  int fd, res;
  ssize_t got;
  char buf[1024];
  res = 0;
  if (!force) {
    if (getenv("HEISENDEBUG")) return false;
    if (IsGenuineCosmo()) return false;
  }
  if (IsWindows()) {
    res = NtGetPeb()->BeingDebugged;
  } else if (IsLinux()) {
    if ((fd = openat$sysv(AT_FDCWD, kProcStatus, O_RDONLY, 0)) != -1) {
      if ((got = read$sysv(fd, buf, sizeof(buf) - sizeof(kGdbPid))) != -1) {
        buf[got] = '\0';
        res =
            atoi(firstnonnull(strstr(buf, kGdbPid), kGdbPid) + strlen(kGdbPid));
      }
      close$sysv(fd);
    }
  }
  return res;
}
