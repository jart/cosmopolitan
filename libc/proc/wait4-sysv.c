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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/rusage.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"

static int sys_wait4_wstatus2linux(int ws) {
  // translate wait status
  if (IsFreebsd() && ws == 19)  // SIGCONT
    return (ws & -0xffff) | 0xffff;
  if (IsXnu() && (ws & 0xff7f) == 0x137f)  // SIGCONT
    return (ws & -0xffff) | 0xffff;
  if ((ws & 0xff) == 0x7f) {
    // tranlate stop signal
    return (ws & -0xffff) | __sig2linux(((ws & 0xff00) >> 8)) << 8 | 0x7f;
  } else {
    // tranlate termination signal
    // this won't do anything to exit statuses
    return (ws & -0x7f) | __sig2linux(ws & 0x7f);
  }
}

int sys_wait4(int pid, int *opt_out_wstatus, int options,
              struct rusage *opt_out_rusage) {
  int rc;
  if ((rc = __sys_wait4(pid, opt_out_wstatus, options, opt_out_rusage)) != -1) {
    if (opt_out_rusage)
      __rusage2linux(opt_out_rusage);
    if (opt_out_wstatus)
      *opt_out_wstatus = sys_wait4_wstatus2linux(*opt_out_wstatus);
  }
  return rc;
}
