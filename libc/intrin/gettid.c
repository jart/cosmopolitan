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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/nexgen32e/gettls.h"
#include "libc/nexgen32e/threaded.h"

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
 * @vforksafe
 */
int gettid(void) {
  int tid;
  if (__tls_enabled && !__vforked) {
    tid = *(int *)(__get_tls() + 0x38);
    if (tid > 0) {
      return tid;
    }
  }
  return sys_gettid();
}
