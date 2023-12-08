/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/errors.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/proc/proc.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int sys_kill_nt(int pid, int sig) {

  // validate api usage
  if (!(0 <= sig && sig <= 64)) {
    return einval();
  }

  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid < -1) {
    pid = -pid;
  }

  // no support for kill all yet
  if (pid == -1) {
    return einval();
  }

  // just call raise() if we're targeting self
  if (pid <= 0 || pid == getpid()) {
    if (sig) {
      if (pid <= 0) {
        struct Dll *e;
        BLOCK_SIGNALS;
        __proc_lock();
        for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e)) {
          TerminateProcess(PROC_CONTAINER(e)->handle, sig);
        }
        __proc_unlock();
        ALLOW_SIGNALS;
      }
      return raise(sig);
    } else {
      return 0;  // ability check passes
    }
  }

  // find existing handle we own for process
  int64_t handle, closeme = 0;
  if (!(handle = __proc_handle(pid))) {
    if ((handle = OpenProcess(kNtProcessTerminate, false, pid))) {
      closeme = handle;
    } else {
      goto OnError;
    }
  }

  // perform actual kill
  // process will report WIFSIGNALED with WTERMSIG(sig)
  bool32 ok = TerminateProcess(handle, sig);
  if (closeme) CloseHandle(closeme);
  if (ok) return 0;

  // handle error
OnError:
  switch (GetLastError()) {
    case kNtErrorInvalidHandle:
    case kNtErrorInvalidParameter:
      return esrch();
    default:
      return eperm();
  }
}

#endif /* __x86_64__ */
