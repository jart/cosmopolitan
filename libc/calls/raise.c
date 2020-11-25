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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/sig.h"

static uint32_t GetCtrlEvent(int sig) {
  switch (sig) {
    case SIGINT:
      return kNtCtrlCEvent;
    case SIGHUP:
      return kNtCtrlCloseEvent;
    case SIGQUIT:
      return kNtCtrlBreakEvent;
    default:
      ExitProcess(128 + sig);
  }
}

/**
 * Sends signal to this process.
 *
 * @param sig can be SIGALRM, SIGINT, SIGTERM, SIGKILL, etc.
 * @return 0 on success or -1 w/ errno
 * @asyncsignalsafe
 */
int raise(int sig) {
  if (sig == SIGTRAP) {
    DebugBreak();
    return 0;
  }
  if (sig == SIGFPE) {
    volatile int x = 0;
    x = 1 / x;
    return 0;
  }
  if (!IsWindows()) {
    return kill$sysv(getpid(), sig, 1);
  } else {
    if (GenerateConsoleCtrlEvent(GetCtrlEvent(sig), 0)) {
      return 0;
    } else {
      return winerr();
    }
  }
}
