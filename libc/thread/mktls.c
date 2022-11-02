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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/thread/spawn.h"
#include "libc/thread/tls.h"

#define I(x) ((intptr_t)x)

void Bzero(void *, size_t) asm("bzero");  // gcc bug

/**
 * Allocates thread-local storage memory for new thread.
 * @return buffer that must be released with free()
 */
char *_mktls(struct CosmoTib **out_tib) {
  char *tls;
  struct CosmoTib *neu, *old;
  __require_tls();

  // allocate memory for tdata, tbss, and tib
  tls = memalign(TLS_ALIGNMENT, I(_tls_size) + sizeof(struct CosmoTib));
  if (!tls) return 0;

  // poison memory between tdata and tbss
  if (IsAsan()) {
    __asan_poison(tls + I(_tdata_size), I(_tbss_offset) - I(_tdata_size),
                  kAsanProtected);
  }

  // initialize tdata and clear tbss
  memmove(tls, _tdata_start, I(_tdata_size));
  Bzero(tls + I(_tbss_offset), I(_tbss_size) + sizeof(struct CosmoTib));

  // set up thread information block
  old = __get_tls();
  neu = (struct CosmoTib *)(tls + I(_tls_size));
  neu->tib_self = neu;
  neu->tib_self2 = neu;
  neu->tib_ftrace = old->tib_ftrace;
  neu->tib_strace = old->tib_strace;
  neu->tib_sigmask = old->tib_sigmask;
  atomic_store_explicit(&neu->tib_tid, -1, memory_order_relaxed);

  if (out_tib) {
    *out_tib = neu;
  }
  return tls;
}
