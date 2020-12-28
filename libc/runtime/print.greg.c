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
#define ShouldUseMsabiAttribute() 1
#include "libc/dce.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/nr.h"

#define WasImported(SLOT) \
  ((void *)*SLOT && *SLOT != (void *)&missingno /* see libc/crt/crt.S */)

static privileged void __print$nt(const void *data, size_t len) {
  int64_t hand;
  uint32_t wrote;
  char xmm[256] forcealign(16);
  savexmm(&xmm[128]);
  hand = __imp_GetStdHandle(kNtStdErrorHandle);
  __imp_WriteFile(hand, data, len, &wrote, NULL);
  __imp_FlushFileBuffers(hand);
  loadxmm(&xmm[128]);
}

/**
 * Prints string, by any means necessary.
 *
 * This function offers a subset of write(STDERR_FILENO) functionality.
 * It's designed to work even when the runtime hasn't initialized, e.g.
 * before _init() gets called.
 *
 * @param len can be computed w/ tinystrlen()
 * @clob nothing except flags
 */
privileged void __print(const void *data, size_t len) {
  int64_t ax, ordinal;
  if (WasImported(__imp_WriteFile)) {
    __print$nt(data, len);
  } else {
    ordinal = __NR_write > 0 ? __NR_write : IsXnu() ? 0x2000004 : 4;
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(ordinal), "D"(STDERR_FILENO), "S"(data), "d"(len)
                 : "rcx", "r11", "memory", "cc");
    if (ax == -1 && !__hostos && !__NR_write) {
      asm volatile("syscall"
                   : "=a"(ax)
                   : "0"(ordinal), "D"(STDERR_FILENO), "S"(data), "d"(len)
                   : "rcx", "r11", "memory", "cc");
    }
  }
}

privileged void __print_string(const char *s) {
  size_t n = 0;
  while (s[n]) ++n;
  __print(s, n);
}
