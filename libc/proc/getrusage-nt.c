/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/rusage.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/nt/accounting.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/iocounters.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/nt/thread.h"
#include "libc/proc/proc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int sys_getrusage_nt(int who, struct rusage *usage) {
  int64_t me;
  struct NtIoCounters iocount;
  struct NtProcessMemoryCountersEx memcount;
  struct NtFileTime ftExit, ftUser, ftKernel, ftCreation;

  if (who == RUSAGE_CHILDREN) {
    if (usage) {
      __proc_lock();
      *usage = __proc.ruchlds;
      __proc_unlock();
    }
    return 0;
  }

  if (who == RUSAGE_SELF || who == RUSAGE_BOTH) {
    me = GetCurrentProcess();
  } else if (who == RUSAGE_THREAD) {
    me = GetCurrentThread();
  } else {
    return einval();
  }

  if (!usage) {
    return 0;
  }

  if (!(who == RUSAGE_THREAD ? GetThreadTimes : GetProcessTimes)(
          me, &ftCreation, &ftExit, &ftKernel, &ftUser) ||
      !GetProcessMemoryInfo(me, &memcount, sizeof(memcount)) ||
      !GetProcessIoCounters(me, &iocount)) {
    return __winerr();
  }

  *usage = (struct rusage){
      .ru_utime = WindowsDurationToTimeVal(ReadFileTime(ftUser)),
      .ru_stime = WindowsDurationToTimeVal(ReadFileTime(ftKernel)),
      .ru_maxrss = memcount.PeakWorkingSetSize / 1024,
      .ru_majflt = memcount.PageFaultCount,
      .ru_inblock = iocount.ReadOperationCount,
      .ru_oublock = iocount.WriteOperationCount,
      .ru_nsignals = atomic_load_explicit(&__sig.count, memory_order_acquire),
  };

  if (who == RUSAGE_BOTH) {
    __proc_lock();
    rusage_add(usage, &__proc.ruchlds);
    __proc_unlock();
  }

  return 0;
}

#endif /* __x86_64__ */
