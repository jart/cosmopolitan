/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "dsp/tty/windex.h"
#include "dsp/tty/tty.h"
#include "libc/nexgen32e/x86feature.h"

extern unsigned windex_avx2(uint16_t *, size_t);
extern unsigned windex_sse4(uint16_t *, size_t);
extern unsigned windex_k8(uint16_t *, size_t);

unsigned (*windex)(uint16_t *, size_t);

__attribute__((__constructor__)) static void init_windex(void) {
#ifdef __x86_64__
  if (X86_HAVE(AVX2)) {
    windex = windex_avx2;
  } else if (X86_HAVE(SSE4_2)) {
    windex = windex_sse4;
  } else {
    windex = windex_k8;
  }
#else
  windex = windex_k8;
#endif
}
