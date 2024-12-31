/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/locale.h"
#include "libc/str/str.h"
#include "libc/thread/tls.h"

#define I(x) ((uintptr_t)x)

static char *_mktls_finish(struct CosmoTib **out_tib, char *mem,
                           struct CosmoTib *tib) {
  struct CosmoTib *old;
  old = __get_tls();
  bzero(tib, sizeof(*tib));
  tib->tib_self = tib;
  tib->tib_self2 = tib;
  tib->tib_ftrace = old->tib_ftrace;
  tib->tib_strace = old->tib_strace;
  tib->tib_sigmask = old->tib_sigmask;
  atomic_init(&tib->tib_ctid, -1);
  if (out_tib)
    *out_tib = tib;
  return mem;
}

static char *_mktls_below(struct CosmoTib **out_tib) {
  size_t siz;
  char *mem, *tls;
  struct CosmoTib *tib;

  // Here's the TLS memory layout on x86_64
  //
  //                           __get_tls()
  //                               │
  //                              %fs OpenBSD/NetBSD
  //            _Thread_local      │
  //     ┌───┬──────────┬──────────┼───┐
  //     │pad│  .tdata  │  .tbss   │tib│
  //     └───┴──────────┴──────────┼───┘
  //                               │
  //    Linux/FreeBSD/Windows/Mac %gs
  //

  siz = ROUNDUP(I(_tls_size) + sizeof(*tib), _Alignof(struct CosmoTib));
  siz = ROUNDUP(siz, _Alignof(struct CosmoTib));
  mem = memalign(_Alignof(struct CosmoTib), siz);

  tib = (struct CosmoTib *)(mem + siz - sizeof(*tib));
  tls = mem + siz - sizeof(*tib) - I(_tls_size);

  // copy in initialized data section
  if (I(_tdata_size)) {
    memcpy(tls, _tdata_start, I(_tdata_size));
  }

  // clear .tbss
  if (I(_tbss_size))
    bzero(tls + I(_tbss_offset), I(_tbss_size));

  // set up thread information block
  return _mktls_finish(out_tib, mem, tib);
}

static char *_mktls_above(struct CosmoTib **out_tib) {

  // Here's the TLS memory layout on aarch64
  //
  //            x28
  //         %tpidr_el0
  //             │
  //             │    _Thread_local
  //         ┌───┼───┬──────────┬──────────┐
  //         │tib│dtv│  .tdata  │  .tbss   │
  //         ├───┴───┴──────────┴──────────┘
  //         │
  //     __get_tls()
  //

  size_t size = ROUNDUP(sizeof(struct CosmoTib), I(_tls_align)) +  //
                ROUNDUP(sizeof(uintptr_t) * 2, I(_tdata_align)) +  //
                ROUNDUP(I(_tdata_size), I(_tbss_align)) +          //
                I(_tbss_size);

  char *mem = memalign(I(_tls_align), size);
  if (!mem)
    return 0;

  struct CosmoTib *tib =
      (struct CosmoTib *)(mem +
                          ROUNDUP(sizeof(struct CosmoTib), I(_tls_align)) -
                          sizeof(struct CosmoTib));

  uintptr_t *dtv = (uintptr_t *)(tib + 1);
  size_t dtv_size = sizeof(uintptr_t) * 2;

  char *tdata = (char *)dtv + ROUNDUP(dtv_size, I(_tdata_align));
  if (I(_tdata_size)) {
    memmove(tdata, _tdata_start, I(_tdata_size));
  }

  char *tbss = tdata + ROUNDUP(I(_tdata_size), I(_tbss_align));
  if (I(_tbss_size)) {
    bzero(tbss, I(_tbss_size));
  }

  dtv[0] = 1;
  dtv[1] = (uintptr_t)tdata;

  return _mktls_finish(out_tib, mem, tib);
}

/**
 * Allocates thread-local storage memory for new thread.
 * @return buffer that must be released with free()
 */
char *_mktls(struct CosmoTib **out_tib) {
#ifdef __x86_64__
  return _mktls_below(out_tib);
#else
  return _mktls_above(out_tib);
#endif
}
