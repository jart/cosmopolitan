/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"

static char tibdefault[64];
extern int __threadcalls_end[];
extern int __threadcalls_start[];

void __enable_tls(void) {
  __initialize_tls(tibdefault);
  *(int *)((char *)tibdefault + 0x38) = gettid();
  *(int *)((char *)tibdefault + 0x3c) = __errno;
  __install_tls(tibdefault);
}

privileged void __enable_threads(void) {
  __threaded = gettid();
  /*
   * _NOPL("__threadcalls", func)
   *
   * we have this
   *
   *     0f 1f 05 b1 19 00 00  nopl func(%rip)
   *
   * we're going to turn it into this
   *
   *     67 67 e8 b1 19 00 00  addr32 addr32 call func
   */
  __morph_begin();
  for (int *p = __threadcalls_start; p < __threadcalls_end; ++p) {
    _base[*p + 0] = 0x67;
    _base[*p + 1] = 0x67;
    _base[*p + 2] = 0xe8;
  }
  __morph_end();
}
