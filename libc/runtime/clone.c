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
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/errfuns.h"

struct WinThread {
  int (*func)(void *);
  void *param;
  void *stack;
};

static noasan textwindows uint32_t winthread(void *param) {
  struct WinThread *wt = param;
  asm volatile("mov\t%%rbp,%%r14\n\t"
               "mov\t%%rsp,%%r15\n\t"
               "xor\t%%ebp,%%ebp\n\t"
               "mov\t%2,%%rsp\n\t"
               "call\t*%0\n\t"
               "mov\t%%r14,%%rbp\n\t"
               "mov\t%%r15,%%rsp"
               : /* no outputs */
               : "m"(wt->func), "D"(wt->param), "m"(wt->stack)
               : "r14", "r15", "memory");
  return 0;
}

/**
 * Creates thread.
 *
 * @note CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND creates thread
 * @note CLONE_VFORK|CLONE_VM|SIGCHLD does vfork()
 * @note SIGCHLD does fork()
 */
privileged int clone(int (*f)(void *), void *stack, int flags, void *arg, ...) {
  int64_t h;
  va_list va;
  intptr_t ax;
  uint32_t tid;
  int32_t *ptid;
  register void *tls asm("r8");
  register int32_t *ctid asm("r10");
  register int (*func)(void *) asm("r9");
  if (IsLinux() || IsNetbsd()) {
    va_start(va, arg);
    ptid = va_arg(va, int32_t *);
    tls = va_arg(va, void *);
    ctid = va_arg(va, int32_t *);
    va_end(va);
    func = f;
    stack = (void *)(((uintptr_t)stack & -16) - 8);
    *(intptr_t *)stack = (intptr_t)arg;
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_clone), "D"(flags), "S"(stack), "d"(ptid),
                   "r"(ctid), "r"(tls), "r"(func)
                 : "rcx", "r11", "memory");
    if (ax) return ax;
    asm volatile("xor\t%%ebp,%%ebp\n\t"
                 "pop\t%%rdi\n\t"
                 "call\t%1"
                 : "=a"(ax)
                 : "r"(func)
                 : "memory");
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_exit), "D"(ax)
                 : "memory");
    unreachable;
  } else if (IsWindows()) {
    if (flags == (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND)) {
      if ((h = CreateThread(0, PAGESIZE, NT2SYSV(winthread),
                            &(struct WinThread){f, arg, stack}, 0, &tid))) {
        CloseHandle(h);
        return tid;
      } else {
        return __winerr();
      }
    } else {
      return einval();
    }
  } else {
    return enosys();
  }
}
