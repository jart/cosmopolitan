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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/proc/proc.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

static textwindows int sys_kill_nt_impl(int pid, int sig) {
  int err;
  bool32 ok;
  struct Dll *e;
  struct Proc *pr = 0;
  for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e)) {
    if (pid == PROC_CONTAINER(e)->pid) {
      pr = PROC_CONTAINER(e);
    }
  }
  if (!pr) {
    return esrch();
  }
  if (sig) {
    err = errno;
    ok = TerminateProcess(pr->handle, sig);
    if (!ok && GetLastError() == kNtErrorAccessDenied) {
      ok = true;  // cargo culting other codebases here
      errno = err;
    }
  }
  return ok ? 0 : -1;
}

textwindows int sys_kill_nt(int pid, int sig) {
  if (!(0 <= sig && sig <= 64)) return einval();

  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid < -1) pid = -pid;

  if (pid == -1) return einval();  // no support for kill all yet

  // If we're targeting current process group then just call raise().
  if (pid <= 0 || pid == getpid()) {
    if (!sig) return 0;  // ability check passes
    return raise(sig);
  }

  int rc;
  uint64_t m;
  m = atomic_exchange(&__get_tls()->tib_sigmask, -1);
  __proc_lock();
  pthread_cleanup_push((void *)__proc_unlock, 0);
  rc = sys_kill_nt_impl(pid, sig);
  pthread_cleanup_pop(true);
  atomic_store_explicit(&__get_tls()->tib_sigmask, m, memory_order_release);

  return rc;
}

#endif /* __x86_64__ */
