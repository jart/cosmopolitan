/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/thread/posixthread.internal.h"
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
__msabi extern typeof(SetThreadContext) *const __imp_SetThreadContext;
__msabi extern typeof(SuspendThread) *const __imp_SuspendThread;
__msabi extern typeof(WriteFile) *const __imp_WriteFile;

int WinThreadLaunch(struct Delivery *, long, int (*)(struct Delivery *), long);

static textwindows unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static textwindows void Log(const char *s) {
#if IsModeDbg()
  __imp_WriteFile(__imp_GetStdHandle(kNtStdErrorHandle), s, StrLen(s), 0, 0);
#endif
}

/**
 * Executes signal handler asynchronously inside other thread.
 *
 * @return 0 on success, or -1 on error
 */
textwindows int _pthread_signal(struct PosixThread *pt, int sig, int sic) {
  int rc = -1;
  intptr_t th;
  if ((th = __imp_OpenThread(
           kNtThreadSuspendResume | kNtThreadGetContext, false,
           atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire)))) {
    uint32_t old_suspend_count;
    if ((old_suspend_count = __imp_SuspendThread(th)) != -1u) {
      if (!old_suspend_count &&
          atomic_load_explicit(&pt->status, memory_order_acquire) <
              kPosixThreadTerminated) {
        struct NtContext nc = {.ContextFlags = kNtContextAll};
        struct Delivery pkg = {0, sig, sic, &nc};
        if (__imp_GetThreadContext(th, &nc)) {
          struct CosmoTib *mytls;
          mytls = __get_tls();
          __set_tls_win32(pt->tib);
          rc = WinThreadLaunch(
              &pkg, 0, __sig_tramp,
              ROUNDDOWN(nc.Rsp - kRedzoneSize, kStackAlign) - 8);
          __imp_SetThreadContext(th, &nc);
          __set_tls_win32(mytls);
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
  return rc;
}

#endif /* __x86_64__ */
