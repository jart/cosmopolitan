/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/stdalign.internal.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"

struct Gadget {
  void (*func)();
  int args[6];
};

static void runcontext(struct Gadget *call, ucontext_t *link) {
  call->func(call->args[0],  //
             call->args[1],  //
             call->args[2],  //
             call->args[3],  //
             call->args[4],  //
             call->args[5]);
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
 * @param uc stores processor state; the caller must:
 *     1. initialize it using getcontext()
 *     2. assign new value to `uc->uc_stack.ss_sp`
 *     3. use `uc->uc_link` to define successor context
 * @param func is the function to call when `uc` is activated;
 *     when `func` returns control passes to the linked context
 *     which if null will result in pthread_exit() being called
 * @param argc is number of `int` arguments for `func` (max 6)
 * @threadsafe
 */
void makecontext(ucontext_t *uc, void func(), int argc, ...) {
  int i;
  va_list va;
  uintptr_t sp;
  struct Gadget *call;
  struct StackFrame *sf;
  assert(argc <= 6u);

  // allocate call
  sp = (uintptr_t)uc->uc_stack.ss_sp;
  sp += uc->uc_stack.ss_size;
  sp -= sizeof(*call);
  sp &= -alignof(*call);
  call = (struct Gadget *)sp;

  // get arguments
  va_start(va, argc);
  call->func = func;
  for (i = 0; i < argc; ++i) {
    call->args[i] = va_arg(va, int);
  }
  va_end(va);

  // construct fake function call on new stack
  //
  // the location where getcontext() was called shall be the previous
  // entry in the backtrace when runcontext was called, e.g.
  //
  //     1000800bf160 423024 systemfive_linux+31
  //     1000800fff90 405299 abort+58
  //     1000800fffb0 40d98c runcontext+42
  //     1000800fffd0 40b308 makecontext_backtrace+20
  //     7fff22a7ff50 40c2d5 testlib_runtestcases+218
  //     7fff22a7ff90 40b1c3 testlib_runalltests+79
  //     7fff22a7ffa0 4038cb main+475
  //     7fff22a7ffe0 403cfb cosmo+69
  //     7fff22a7fff0 4034e2 _start+137
  //
  // is about what it should look like.
  sp &= -(sizeof(uintptr_t) * 2);
#ifdef __x86__
  *(uintptr_t *)(sp -= sizeof(uintptr_t)) = uc->uc_mcontext.PC;
#elif defined(__aarch64__)
  *(uintptr_t *)(sp -= sizeof(uintptr_t)) = uc->uc_mcontext.regs[30];
  *(uintptr_t *)(sp -= sizeof(uintptr_t)) = uc->uc_mcontext.regs[29];
  uc->uc_mcontext.BP = uc->uc_mcontext.SP;
#else
#error "unsupported architecture"
#endif

  // program context
  uc->uc_mcontext.SP = sp;
  uc->uc_mcontext.PC = (uintptr_t)runcontext;
  uc->uc_mcontext.ARG0 = (uintptr_t)call;
  uc->uc_mcontext.ARG1 = (uintptr_t)uc->uc_link;
}
