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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/errors.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/proc/proc.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int sys_kill_nt(int pid, int sig) {

  // validate api usage
  if (!(0 <= sig && sig <= 64))
    return einval();

  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid < -1)
    pid = -pid;

  // no support for kill all yet
  if (pid == -1)
    return einval();

  // just call raise() if we're targeting self
  if (pid <= 0 || pid == getpid()) {
    if (sig) {
      if (pid <= 0) {
        // if pid is 0 or -1 then kill the processes beneath us too.
        // this isn't entirely right but it's closer to being right.
        // having this behavior is helpful for servers like redbean.
        struct Dll *e;
        BLOCK_SIGNALS;
        __proc_lock();
        for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e)) {
          atomic_ulong *sigproc;
          struct Proc *pr = PROC_CONTAINER(e);
          if (sig != 9 && (sigproc = __sig_map_process(pid, kNtOpenExisting))) {
            atomic_fetch_or_explicit(sigproc, 1ull << (sig - 1),
                                     memory_order_release);
          } else {
            TerminateProcess(pr->handle, sig);
          }
        }
        __proc_unlock();
        ALLOW_SIGNALS;
      }
      return raise(sig);
    } else {
      return 0;  // ability check passes
    }
  }

  // attempt to signal via shared memory file
  //
  // now that we know the process exists, if it has a shared memory file
  // then we can be reasonably certain it's a cosmo process which should
  // be trusted to deliver its signal, unless it's a nine exterminations
  if (pid > 0 && sig != 9) {
    atomic_ulong *sigproc;
    if ((sigproc = __sig_map_process(pid, kNtOpenExisting))) {
      if (sig > 0)
        atomic_fetch_or_explicit(sigproc, 1ull << (sig - 1),
                                 memory_order_release);
      UnmapViewOfFile(sigproc);
      if (sig != 9)
        return 0;
    }
  }

  // find existing handle we own for process
  //
  // this step should come first to verify process existence. this is
  // because there's no guarantee that just because the shared memory
  // file exists, the process actually exists.
  int64_t handle, closeme = 0;
  if (!(handle = __proc_handle(pid))) {
    if (!(handle = OpenProcess(kNtProcessTerminate, false, pid)))
      return eperm();
    closeme = handle;
  }

  // perform actual kill
  // process will report WIFSIGNALED with WTERMSIG(sig)
  if (sig != 9)
    STRACE("warning: kill() sending %G via terminate", sig);
  bool32 ok = TerminateProcess(handle, sig);
  if (closeme)
    CloseHandle(closeme);
  if (ok)
    return 0;
  return esrch();
}

#endif /* __x86_64__ */
