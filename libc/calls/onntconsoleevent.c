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
#include "libc/calls/sig.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/log/libfatal.internal.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"

#ifdef __x86_64__

// WIN32 doesn't have the System V red-zone. Microsoft says they reserve
// the right to trample all over it. so we technically don't need to use
// this value. it's just not clear how common it is for WIN32 to clobber
// the red zone, which means broken code could seem to mostly work which
// means it's better that we're not the ones responsible for breaking it
#define kRedzoneSize 128

// Both Microsoft and the Fifth Bell System agree on this one.
#define kStackAlign 16

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;
__msabi extern typeof(GetLastError) *const __imp_GetLastError;
__msabi extern typeof(GetStdHandle) *const __imp_GetStdHandle;
__msabi extern typeof(GetThreadContext) *const __imp_GetThreadContext;
__msabi extern typeof(OpenThread) *const __imp_OpenThread;
__msabi extern typeof(ResumeThread) *const __imp_ResumeThread;
__msabi extern typeof(SuspendThread) *const __imp_SuspendThread;
__msabi extern typeof(WriteFile) *const __imp_WriteFile;

int WinThreadLaunch(int, int, int (*)(int, int), intptr_t);

static unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static void Log(const char *s) {
#ifndef NDEBUG
  __imp_WriteFile(__imp_GetStdHandle(kNtStdErrorHandle), s, StrLen(s), 0, 0);
#endif
}

static int GetSig(uint32_t dwCtrlType) {
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

__msabi textwindows dontinstrument dontasan dontubsan bool32
__onntconsoleevent(uint32_t dwCtrlType) {

  // the signal to be delivered
  int sig = GetSig(dwCtrlType);
  int sic = SI_KERNEL;

  // if we don't have tls, then we can't hijack a safe stack from a
  // thread so just try our luck punting the signal to the next i/o
  if (!__tls_enabled) {
    goto PuntSignal;
  }

  // we're on a stack that's owned by win32. to make matters worse,
  // win32 spawns a totally new thread just to invoke this handler.
  // that means most of the cosmo runtime is broken right now which
  // means we can't call the user signal handler safely. what we'll
  // do instead is pick a posix thread at random to hijack, pretend
  // to be that thread, use its stack, and then deliver this signal
  // asynchronously if it isn't blocked. hopefully it won't longjmp
  // because once the handler returns, we'll restore the old thread
  bool gotsome = false;
  pthread_spin_lock(&_pthread_lock);
  for (struct Dll *e = dll_first(_pthread_list); e && !gotsome;
       e = dll_next(_pthread_list, e)) {
    struct PosixThread *pt = POSIXTHREAD_CONTAINER(e);
    int tid = atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire);
    if (tid <= 0) continue;  // -1 means spawning, 0 means terminated
    if (pt->tib->tib_sigmask & (1ull << (sig - 1))) continue;  // blocked
    intptr_t th;
    if ((th = __imp_OpenThread(kNtThreadSuspendResume | kNtThreadGetContext,
                               false, tid))) {
      uint32_t old_suspend_count = __imp_SuspendThread(th);
      if (old_suspend_count != -1u) {
        if (!old_suspend_count &&
            atomic_load_explicit(&pt->status, memory_order_acquire) <
                kPosixThreadTerminated) {
          struct NtContext ctx;
          __repstosb(&ctx, 0, sizeof(ctx));
          ctx.ContextFlags = kNtContextControl;
          if (__imp_GetThreadContext(th, &ctx)) {
            gotsome = true;
            pthread_spin_unlock(&_pthread_lock);
            __set_tls_win32(pt->tib);
            WinThreadLaunch(sig, sic, __sig_raise,
                            ROUNDDOWN(ctx.Rsp - kRedzoneSize, kStackAlign) - 8);
          } else {
            Log("GetThreadContext failed\n");
          }
        }
        __imp_ResumeThread(th);
      } else {
        Log("SuspendThread failed\n");
      }
      __imp_CloseHandle(th);
    } else {
      Log("OpenThread failed\n");
    }
  }

  if (!gotsome) {
    pthread_spin_unlock(&_pthread_lock);
  PuntSignal:
    __sig_add(0, sig, sic);
  }
  return true;
}

#endif /* __x86_64__ */
