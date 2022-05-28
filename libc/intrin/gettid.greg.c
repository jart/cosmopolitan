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
#include "libc/dce.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"

__msabi extern typeof(GetCurrentThreadId) *const __imp_GetCurrentThreadId;

/**
 * Returns current thread id.
 *
 * On Linux, and Linux only, this is guaranteed to be equal to getpid()
 * if this is the main thread. On NetBSD, gettid() for the main thread
 * is always 1.
 *
 * This function issues a system call. That stops being the case as soon
 * as __install_tls() is called.  That'll happen automatically, when you
 * call clone() and provide the TLS parameter. We assume that when a TLS
 * block exists, then
 *
 *     *(int *)(__get_tls() + 0x38)
 *
 * will contain the thread id. Therefore when issuing clone() calls, the
 * `CLONE_CHILD_SETTID` and `CLONE_CHILD_CLEARTID` flags should use that
 * index as its `ctid` memory.
 *
 *     gettid (single threaded) l:       126𝑐        41𝑛𝑠
 *     gettid (tls enabled)     l:         2𝑐         1𝑛𝑠
 *
 * The TLS convention is important for reentrant lock performance.
 *
 * @return thread id greater than zero or -1 w/ errno
 * @asyncsignalsafe
 * @threadsafe
 */
privileged int gettid(void) {
  int rc;
  int64_t wut;
  struct WinThread *wt;

  if (__tls_enabled) {
    rc = *(int *)(__get_tls_inline() + 0x38);
    return rc;
  }

  if (IsWindows()) {
    return __imp_GetCurrentThreadId();
  }

  if (IsLinux()) {
    asm("syscall"
        : "=a"(rc)  // man says always succeeds
        : "0"(186)  // __NR_gettid
        : "rcx", "r11", "memory");
    return rc;
  }

  if (IsXnu()) {
    asm("syscall"              // xnu/osfmk/kern/ipc_tt.c
        : "=a"(rc)             // assume success
        : "0"(0x1000000 | 27)  // Mach thread_self_trap()
        : "rcx", "r11", "memory", "cc");
    return rc;
  }

  if (IsOpenbsd()) {
    asm("syscall"
        : "=a"(rc)  // man says always succeeds
        : "0"(299)  // getthrid()
        : "rcx", "r11", "memory", "cc");
    return rc;
  }

  if (IsNetbsd()) {
    asm("syscall"
        : "=a"(rc)  // man says always succeeds
        : "0"(311)  // _lwp_self()
        : "rcx", "rdx", "r11", "memory", "cc");
    return rc;
  }

  if (IsFreebsd()) {
    asm("syscall"
        : "=a"(rc),  // only fails w/ EFAULT, which isn't possible
          "=m"(wut)  // must be 64-bit
        : "0"(432),  // thr_self()
          "D"(&wut)  // but not actually 64-bit
        : "rcx", "r11", "memory", "cc");
    return wut;  // narrowing intentional
  }

  return __pid;
}
