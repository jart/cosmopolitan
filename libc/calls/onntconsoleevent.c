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
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#ifdef __x86_64__

__msabi extern typeof(GetStdHandle) *const __imp_GetStdHandle;
__msabi extern typeof(WriteFile) *const __imp_WriteFile;

static textwindows unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static textwindows void Log(const char *s) {
#ifndef NDEBUG
  __imp_WriteFile(__imp_GetStdHandle(kNtStdErrorHandle), s, StrLen(s), 0, 0);
#endif
}

static textwindows int GetSig(uint32_t dwCtrlType) {
  switch (dwCtrlType) {
    case kNtCtrlCEvent:
      return SIGINT;
    case kNtCtrlBreakEvent:
      return SIGQUIT;
    case kNtCtrlCloseEvent:
    case kNtCtrlLogoffEvent:    // only received by services
    case kNtCtrlShutdownEvent:  // only received by services
      return SIGHUP;
    default:
      __builtin_unreachable();
  }
}

__msabi textwindows bool32 __onntconsoleevent(uint32_t dwCtrlType) {

  // we're on a stack that's owned by win32. to make matters worse,
  // win32 spawns a totally new thread just to invoke this handler.
  int sig = GetSig(dwCtrlType);
  if (__sighandrvas[sig] == (uintptr_t)SIG_IGN) {
    return true;
  }

  // if we don't have tls, then we can't hijack a safe stack from a
  // thread so just try our luck punting the signal to the next i/o
  if (!__tls_enabled) {
    __sig_add(0, sig, SI_KERNEL);
    return true;
  }

  pthread_spin_lock(&_pthread_lock);

  // before we get asynchronous, let's try to find a thread that is
  // currently blocked on io which we can interrupt with our signal
  // this is important, because if we we asynchronously interrupt a
  // thread that's calling ReadFile() by suspending / resuming then
  // the io operation will report end of file (why) upon resumation
  struct Dll *e;
  for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
    struct PosixThread *pt = POSIXTHREAD_CONTAINER(e);
    int tid = atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire);
    if (tid <= 0) continue;  // -1 means spawning, 0 means terminated
    if (pt->tib->tib_sigmask & (1ull << (sig - 1))) continue;  // masked
    if (pt->flags & PT_BLOCKED) {
      pthread_spin_unlock(&_pthread_lock);
      __sig_add(0, sig, SI_KERNEL);
      return true;
    }
  }

  // limbo means most of the cosmo runtime is totally broken, which
  // means we can't call the user signal handler safely. what we'll
  // do instead is pick a posix thread at random to hijack, pretend
  // to be that thread, use its stack, and then deliver this signal
  // asynchronously if it isn't blocked. hopefully it won't longjmp
  // because once the handler returns, we'll restore the old thread
  // going asynchronous is heavy but it's the only way to stop code
  // that does stuff like scientific computing, which are cpu-bound
  for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
    struct PosixThread *pt = POSIXTHREAD_CONTAINER(e);
    int tid = atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire);
    if (tid <= 0) continue;  // -1 means spawning, 0 means terminated
    if (pt->tib->tib_sigmask & (1ull << (sig - 1))) continue;  // masked
    pthread_spin_unlock(&_pthread_lock);
    if (_pthread_signal(pt, sig, SI_KERNEL) == -1) {
      __sig_add(0, sig, SI_KERNEL);
    }
    return true;
  }

  pthread_spin_unlock(&_pthread_lock);
  return true;
}

#endif /* __x86_64__ */
