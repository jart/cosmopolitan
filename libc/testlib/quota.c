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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/errno.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

static noasan relegated uint64_t CountMappedBytes(void) {
  size_t i;
  uint64_t x, y;
  for (x = i = 0; i < _mmi.i; ++i) {
    y = _mmi.p[i].y - _mmi.p[i].x;
    x += (y + 1) << 16;
  }
  return x;
}

static relegated void DieBecauseOfQuota(char *p, int rc, const char *message) {
  int e;
  char hostname[32];
  e = errno;
  __restore_tty(2);
  __stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  p = __stpcpy(p, message);
  p = __stpcpy(p, " on ");
  p = __stpcpy(p, hostname);
  p = __stpcpy(p, " pid ");
  p = __intcpy(p, __getpid());
  p = __stpcpy(p, "\n");
  __write(__fatalbuf, p - __fatalbuf);
  PrintBacktraceUsingSymbols(2, 0, GetSymbolTable());
  exit(rc);
}

static relegated void OnXcpu(int sig) {
  DieBecauseOfQuota(__fatalbuf, 23, "\n\nSIGXCPU: ran out of cpu");
}

static relegated void OnXfsz(int sig) {
  DieBecauseOfQuota(__fatalbuf, 25, "\n\nSIGXFSZ: exceeded maximum file size");
}

relegated void __oom_hook(size_t request) {
  int e;
  char *p;
  uint64_t toto, newlim;
  struct MallocStats stats;
  e = errno;
  p = __fatalbuf;
  toto = CountMappedBytes();
  stats = dlmalloc_stats(g_dlmalloc);
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "WE REQUIRE MORE VESPENE GAS");
  if (e != ENOMEM) {
    p = __stpcpy(p, " (");
    p = __stpcpy(p, strerror(e));
    p = __stpcpy(p, ")");
  }
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "mmap last request       = ");
  p = __intcpy(p, request);
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "mmapped system bytes    = ");
  p = __intcpy(p, toto);
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "malloc max system bytes = ");
  p = __intcpy(p, stats.maxfp);
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "malloc system bytes     = ");
  p = __intcpy(p, stats.fp);
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "malloc in use bytes     = ");
  p = __intcpy(p, stats.used);
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "\n");
  if (IsRunningUnderMake()) {
    newlim = toto + request;
    newlim += newlim >> 1;
    newlim = roundup2pow(newlim);
    p = __stpcpy(p, "FIX CODE OR TUNE QUOTA += -M");
    p = __intcpy(p, newlim / (1024 * 1024));
    p = __stpcpy(p, "m\n");
  }
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "THE STRAW THAT BROKE THE CAMEL'S BACK\n");
  DieBecauseOfQuota(p, 42, "MAP_FAILED: exceeded memory quota");
}

static textstartup void InstallQuotaHandlers(void) {
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = OnXcpu;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGXCPU, &sa, 0);
  sa.sa_handler = OnXfsz;
  sigaction(SIGXFSZ, &sa, 0);
  GetSymbolTable(); /* for effect in case we oom */
}

const void *const testlib_quota_handlers[] initarray = {
    InstallQuotaHandlers,
};
