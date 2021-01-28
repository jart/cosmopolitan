/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/process.h"
#include "libc/sysv/errfuns.h"

textwindows int kill$nt(int pid, int sig) {
  int target;
  uint32_t event;
  if (!pid) return raise(sig);
  if ((pid > 0 && __isfdkind(pid, kFdProcess)) ||
      (pid < 0 && __isfdkind(-pid, kFdProcess))) {
    target = GetProcessId(g_fds.p[ABS(pid)].handle);
  } else {
    target = pid;
  }
  if (target == GetCurrentProcessId()) {
    return raise(sig);
  } else {
    switch (sig) {
      case SIGINT:
        event = kNtCtrlCEvent;
      case SIGHUP:
        event = kNtCtrlCloseEvent;
      case SIGQUIT:
        event = kNtCtrlBreakEvent;
      default:
        return einval();
    }
    if (GenerateConsoleCtrlEvent(event, target)) {
      return 0;
    } else {
      return __winerr();
    }
  }
}
