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
#include "libc/assert.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/stdalign.internal.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"

typedef double vect __attribute__((__vector_size__(16), __aligned__(16)));

struct Gadget {
  void (*func)(long, long, long, long, long, long,  //
               vect, vect, vect, vect, vect, vect);
  long longs[6];
  vect vects[6];
};

static void runcontext(struct Gadget *call, ucontext_t *link) {
  call->func(call->longs[0], call->longs[1], call->longs[2], call->longs[3],
             call->longs[4], call->longs[5], call->vects[0], call->vects[1],
             call->vects[2], call->vects[3], call->vects[4], call->vects[5]);
  if (link) {
    setcontext(link);
    abort();
  } else {
    pthread_exit(0);
  }
}

/**
 * Creates coroutine gadget, e.g.
 *
 *     ucontext_t uc;
 *     getcontext(&uc);
 *     uc.uc_link = 0;
 *     uc.uc_stack.ss_sp = NewCosmoStack();
 *     uc.uc_stack.ss_size = GetStackSize();
 *     makecontext(&uc, exit, 1, 42);
 *     setcontext(&uc);
 *
 * Is equivalent to:
 *
 *     exit(42);
 *
 * The safest way to allocate stack memory is to use NewCosmoStack() and
 * GetStackSize(), which will mmap() a fresh region of memory per a link
 * time configuration, mprotect() some guard pages at the bottom, poison
 * them if ASAN is in play, and then tell the OS that it's stack memory.
 * If that's overkill for your use case, then you could potentially pass
 * stacks as small as 1024 bytes; however they need to come from a stack
 * allocation Cosmo granted to your main process and threads. It needn't
 * be aligned, since this function takes care of that automatically. The
 * address selected shall be `uc_stack.ss_ip + uc_stack.ss_size` and all
 * the action happens beneath that address.
 *
 * On AMD64 and ARM64 you may pass up to six `long` integer args, and up
 * to six vectors (e.g. double, floats, __m128i, uint8x16_t). Thou shall
 * not call code created by Microsoft compilers, even though this should
 * work perfectly fine on Windows, as it is written in the System V ABI,
 * which specifies your parameters are always being passed in registers.
 *
 * @param uc stores processor state; the caller must have:
 *     1. initialized it using `getcontext(uc)`
 *     2. allocated new values for `uc->uc_stack`
 *     3. specified a successor context in `uc->uc_link`
 * @param func is the function to call when `uc` is activated;
 *     when `func` returns, control is passed to `uc->uc_link`,
 *     which if null will result in pthread_exit() being called
 * @param argc is effectively ignored (see notes above)
 * @see setcontext(), getcontext(), swapcontext()
 */
void makecontext(ucontext_t *uc, void *func, int argc, ...) {
  va_list va;
  long sp, sb;
  struct Gadget *call;

  // allocate call
  sp = sb = (long)uc->uc_stack.ss_sp;
  sp += uc->uc_stack.ss_size;
  sp -= 16;  // openbsd:stackbound
  sp -= sizeof(*call);
  sp &= -alignof(struct Gadget);
  call = (struct Gadget *)sp;

  // get arguments
  call->func = func;
  va_start(va, argc);
  call->longs[0] = va_arg(va, long);
  call->longs[1] = va_arg(va, long);
  call->longs[2] = va_arg(va, long);
  call->longs[3] = va_arg(va, long);
  call->longs[4] = va_arg(va, long);
  call->longs[5] = va_arg(va, long);
  call->vects[0] = va_arg(va, vect);
  call->vects[1] = va_arg(va, vect);
  call->vects[2] = va_arg(va, vect);
  call->vects[3] = va_arg(va, vect);
  call->vects[4] = va_arg(va, vect);
  call->vects[5] = va_arg(va, vect);
  va_end(va);

  // constructs fake function call on new stack
  //
  // the location where getcontext() was called shall be the previous
  // entry in the backtrace when runcontext was called, e.g.
  //
  //     1000800fff90 405299 func+58
  //     1000800fffb0 40d98c runcontext+42
  //     1000800fffd0 40b308 makecontext_backtrace+20
  //     7fff22a7ff50 40c2d5 testlib_runtestcases+218
  //     7fff22a7ff90 40b1c3 testlib_runalltests+79
  //     7fff22a7ffa0 4038cb main+475
  //     7fff22a7ffe0 403cfb cosmo+69
  //     7fff22a7fff0 4034e2 _start+137
  //
  // is about what it should look like.
  sp &= -(sizeof(long) * 2);
#ifdef __x86__
  *(long *)(sp -= sizeof(long)) = uc->uc_mcontext.PC;
#elif defined(__aarch64__)
  *(long *)(sp -= sizeof(long)) = uc->uc_mcontext.regs[30];
  *(long *)(sp -= sizeof(long)) = uc->uc_mcontext.regs[29];
  uc->uc_mcontext.BP = uc->uc_mcontext.SP;
#else
#error "unsupported architecture"
#endif

  // program context
  uc->uc_mcontext.SP = sp;
  uc->uc_mcontext.PC = (long)runcontext;
  uc->uc_mcontext.ARG0 = (long)call;
  uc->uc_mcontext.ARG1 = (long)uc->uc_link;
}
