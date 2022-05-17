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
#include "libc/intrin/threaded.h"
#include "libc/nt/thread.h"

/**
 * Returns current thread id.
 * @asyncsignalsafe
 */
privileged int gettid(void) {
  int rc;
  int64_t wut;
  struct WinThread *wt;

  if (IsWindows()) {
    return GetCurrentThreadId();
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

  return getpid();
}
