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
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/dlmalloc/dlmalloc.h"

static noasan noubsan relegated uint64_t CountMappedBytes(void) {
  size_t i;
  uint64_t x, y;
  for (x = i = 0; i < _mmi.i; ++i) {
    y = _mmi.p[i].y - _mmi.p[i].x;
    x += (y + 1) << 16;
  }
  return x;
}

static relegated void DieBecauseOfQuota(int rc, const char *message) {
  int e = errno;
  char hostname[32];
  stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  kprintf("%s on %s pid %d\n", message, hostname, (long)getpid());
  PrintBacktraceUsingSymbols(2, 0, GetSymbolTable());
  _Exitr(rc);
}

static relegated void OnXcpu(int sig) {
  __restore_tty();
  DieBecauseOfQuota(23, "\n\nSIGXCPU: ran out of cpu");
}

static relegated void OnXfsz(int sig) {
  __restore_tty();
  DieBecauseOfQuota(25, "\n\nSIGXFSZ: exceeded maximum file size");
}

relegated void __oom_hook(size_t request) {
  int e;
  uint64_t toto, newlim;
  __restore_tty();
  e = errno;
  toto = CountMappedBytes();
  kprintf("\n\nWE REQUIRE MORE VESPENE GAS");
  if (e != ENOMEM) kprintf(" (%s)", strerror(e));
  if (IsRunningUnderMake()) {
    newlim = toto + request;
    newlim += newlim >> 1;
    newlim = _roundup2pow(newlim);
    kprintf("FIX CODE OR TUNE QUOTA += -M%dm\n", newlim / (1024 * 1024));
  }
  kprintf("\n");
  __print_maps();
  kprintf("\nTHE STRAW THAT BROKE THE CAMEL'S BACK\n");
  PrintBacktraceUsingSymbols(2, 0, GetSymbolTable());
  PrintSystemMappings(2);
  _Exitr(42);
}

static textstartup void InstallQuotaHandlers(void) {
  int e;
  struct sigaction sa;
  e = errno;
  sa.sa_flags = 0;
  sa.sa_handler = OnXcpu;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGXCPU, &sa, 0);
  sa.sa_handler = OnXfsz;
  sigaction(SIGXFSZ, &sa, 0);
  GetSymbolTable(); /* for effect in case we oom */
  errno = e;
}

const void *const testlib_quota_handlers[] initarray = {
    InstallQuotaHandlers,
};
