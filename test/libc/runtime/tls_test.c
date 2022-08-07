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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"

_Thread_local int x;
_Thread_local int y = 40;
int z = 2;

void PrintInfo(void) {
  kprintf("_tdata_size  = %d\n", _tdata_size);
  kprintf("_tls_size    = %d\n", _tls_size);
  kprintf("_tls_content = %d\n", _tls_content);
  kprintf("__data_start = %p\n", __data_start);
  kprintf("__data_end   = %p\n", __data_end);
  kprintf("_tdata_start = %p\n", _tdata_start);
  kprintf("_tdata_end   = %p\n", _tdata_end);
  kprintf("_tbss_start  = %p\n", _tbss_start);
  kprintf("_tbss_end    = %p\n", _tbss_end);
  kprintf("&y           = %p\n", &y);
  kprintf("__bss_start  = %p\n", __bss_start);
  kprintf("__bss_end    = %p\n", __bss_end);
}

TEST(tls, test) {
  EXPECT_EQ(42, x + y + z);
}
