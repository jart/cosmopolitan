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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/intrin/atomic.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/xed/x86.h"

static volatile _Thread_local int gotsignal;

static void ContinueOnError(int sig, siginfo_t *si, void *vctx) {
  struct ucontext *ctx = vctx;
  gotsignal = sig;
#ifdef __aarch64__
  ctx->uc_mcontext.pc += 4;
#elif defined(__x86_64__)
  struct XedDecodedInst xedd;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
#else
#error "unsupported architecture"
#endif /* __x86_64__ */
}

/**
 * Returns true if byte at address `p` is readable.
 *
 * This function temporarily catches `SIGSEGV` and `SIGBUS` to recover
 * on error. It then attempts a volatile read and if it faults, then
 * this function shall return false. The value at `p` isn't considered.
 */
dontasan bool testlib_memoryexists(const void *p) {
  volatile char c;
  const atomic_char *mem = p;
  struct sigaction old[2];
  struct sigaction sa = {
      .sa_sigaction = ContinueOnError,
      .sa_flags = SA_SIGINFO,
  };
  gotsignal = 0;
  npassert(!sigaction(SIGSEGV, &sa, old + 0));
  npassert(!sigaction(SIGBUS, &sa, old + 1));
  c = atomic_load(mem);
  (void)c;
  npassert(!sigaction(SIGBUS, old + 1, 0));
  npassert(!sigaction(SIGSEGV, old + 0, 0));
  return !gotsignal;
}
