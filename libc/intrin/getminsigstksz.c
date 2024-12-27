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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/getauxval.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ss.h"

long __get_minsigstksz(void) {
  struct AuxiliaryValue av;
  av = __getauxval(AT_MINSIGSTKSZ);
  if (av.isfound) {
    long res = av.value;
    if (!IsLinux())
      res += sizeof(struct ucontext) + sizeof(struct siginfo) + 128;
    if (res < _MINSIGSTKSZ)
      res = _MINSIGSTKSZ;
    return res;
  } else {
    // _MINSIGSTKSZ takes these things into consideration:
    //
    // 1. The platform definition of MINSIGSTKSZ. This will probably be
    //    enforced by the kernel when calling sys_sigaltstack(). On ARM
    //    platforms this might be several kilobytes larger than x86. On
    //    Linux they really want you to use AT_MINSIGSTKSZ instead. The
    //    kernel should ideally set this to be the number of bytes that
    //    get subtracted from the stack pointer when delivering signals
    //    meaning that if you use this for a stack size your handler is
    //    called successfully but if it uses the stack then it'll crash
    //
    // 2. Cosmo sigenter overhead. On non-Linux OSes the kernel calls a
    //    trampoline in the libc runtime, which translates the platform
    //    specific signal frame to the Linux memory layout. It means we
    //    need to push ~1024 extra bytes on the stack to call a handler
    //
    // 3. Sanity testing. Assume we use sysconf(_SC_MINSIGSTKSZ) + 2048
    //    as our stack size (see stackoverflow1_test.c). Then we should
    //    have enough room to use kprintf() from our signal handler. If
    //    that isn't the case, then this should be increased a bit more
    //    noting that if 1024 is used then kprintf should print refusal
    //
    return _MINSIGSTKSZ;
  }
}
