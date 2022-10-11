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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_setpriority_nt(int which, unsigned pid, int nice) {
  int rc;
  uint32_t tier;
  int64_t h, closeme = -1;

  if (which != PRIO_PROCESS) {
    return einval();
  }

  if (!pid || pid == getpid()) {
    h = GetCurrentProcess();
  } else if (__isfdkind(pid, kFdProcess)) {
    h = g_fds.p[pid].handle;
  } else {
    h = OpenProcess(kNtProcessSetInformation | kNtProcessQueryInformation,
                    false, pid);
    if (!h) return __winerr();
    closeme = h;
  }

  if (nice <= -15) {
    tier = kNtRealtimePriorityClass;
  } else if (nice <= -9) {
    tier = kNtHighPriorityClass;
  } else if (nice <= -3) {
    tier = kNtAboveNormalPriorityClass;
  } else if (nice <= 3) {
    tier = kNtNormalPriorityClass;
  } else if (nice <= 12) {
    tier = kNtBelowNormalPriorityClass;
  } else {
    tier = kNtIdlePriorityClass;
  }

  if (SetPriorityClass(h, tier)) {
    rc = 0;
  } else {
    rc = __winerr();
  }

  if (closeme != -1) {
    CloseHandle(closeme);
  }

  return rc;
}
