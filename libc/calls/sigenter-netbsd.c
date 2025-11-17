/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo-meta.internal.h"
#include "libc/calls/struct/siginfo-netbsd.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/ucontext-netbsd.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/pib.h"

#ifdef __x86_64__

__privileged void __sigenter_netbsd(int sig, struct siginfo_netbsd *si,
                                    struct ucontext_netbsd *ctx) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  ucontext_t uc;
  CheckLargeStackAllocation(&uc, sizeof(uc));
#pragma GCC pop_options
  int rva, flags;
  siginfo_t si2;
  struct CosmoPib *pib = __get_pib();
  sig = __sig2linux(sig);
  rva = pib->sighandrvas[sig - 1];
  if (rva >= kSigactionMinRva) {
    flags = pib->sighandflags[sig - 1];

    // sigaltstack() has this issue on xnu and netbsd where if we
    // longjmp() out of the signal (which causes sigreturn() to be
    // bypassed below) then the SS_ONSTACK state will stick permanently.
    // to solve this our sigsetjmp() implementation will longjmp() here.
    int jbval;
    jmp_buf jmpbuf;
    struct CosmoTib *tib = __get_tls_privileged();
    tib->tib_sigjmpbuf = jmpbuf;
    if (!(jbval = _setjmp(jmpbuf))) {

      // call the signal handler
      if (~flags & SA_SIGINFO) {
        ((sigaction_f)(__executable_start + rva))(sig, 0, 0);
      } else {
        __repstosb(&uc, 0, sizeof(uc));
        __siginfo2cosmo(&si2, (void *)si);
        uc.uc_mcontext.fpregs = &uc.__fpustate;
        uc.uc_stack.ss_sp = ctx->uc_stack.ss_sp;
        uc.uc_stack.ss_size = ctx->uc_stack.ss_size;
        uc.uc_stack.ss_flags = ctx->uc_stack.ss_flags;
        uc.uc_sigmask = __mask2linux(ctx->uc_sigmask[0] |
                                     (uint64_t)ctx->uc_sigmask[0] << 32);
        uc.uc_mcontext.rdi = ctx->uc_mcontext.rdi;
        uc.uc_mcontext.rsi = ctx->uc_mcontext.rsi;
        uc.uc_mcontext.rdx = ctx->uc_mcontext.rdx;
        uc.uc_mcontext.rcx = ctx->uc_mcontext.rcx;
        uc.uc_mcontext.r8 = ctx->uc_mcontext.r8;
        uc.uc_mcontext.r9 = ctx->uc_mcontext.r9;
        uc.uc_mcontext.rax = ctx->uc_mcontext.rax;
        uc.uc_mcontext.rbx = ctx->uc_mcontext.rbx;
        uc.uc_mcontext.rbp = ctx->uc_mcontext.rbp;
        uc.uc_mcontext.r10 = ctx->uc_mcontext.r10;
        uc.uc_mcontext.r11 = ctx->uc_mcontext.r11;
        uc.uc_mcontext.r12 = ctx->uc_mcontext.r12;
        uc.uc_mcontext.r13 = ctx->uc_mcontext.r13;
        uc.uc_mcontext.r14 = ctx->uc_mcontext.r14;
        uc.uc_mcontext.r15 = ctx->uc_mcontext.r15;
        uc.uc_mcontext.trapno = ctx->uc_mcontext.trapno;
        uc.uc_mcontext.fs = ctx->uc_mcontext.fs;
        uc.uc_mcontext.gs = ctx->uc_mcontext.gs;
        uc.uc_mcontext.err = ctx->uc_mcontext.err;
        uc.uc_mcontext.rip = ctx->uc_mcontext.rip;
        uc.uc_mcontext.rsp = ctx->uc_mcontext.rsp;
        __repmovsb(uc.uc_mcontext.fpregs, &ctx->uc_mcontext.__fpregs,
                   sizeof(ctx->uc_mcontext.__fpregs));
        ((sigaction_f)(__executable_start + rva))(sig, &si2, &uc);
        ctx->uc_stack.ss_sp = uc.uc_stack.ss_sp;
        ctx->uc_stack.ss_size = uc.uc_stack.ss_size;
        ctx->uc_stack.ss_flags = uc.uc_stack.ss_flags;
        sigset_t mask2 = __linux2mask(uc.uc_sigmask);
        ctx->uc_sigmask[0] = mask2;
        ctx->uc_sigmask[1] = mask2 >> 32;
        ctx->uc_mcontext.rdi = uc.uc_mcontext.rdi;
        ctx->uc_mcontext.rsi = uc.uc_mcontext.rsi;
        ctx->uc_mcontext.rdx = uc.uc_mcontext.rdx;
        ctx->uc_mcontext.rcx = uc.uc_mcontext.rcx;
        ctx->uc_mcontext.r8 = uc.uc_mcontext.r8;
        ctx->uc_mcontext.r9 = uc.uc_mcontext.r9;
        ctx->uc_mcontext.rax = uc.uc_mcontext.rax;
        ctx->uc_mcontext.rbx = uc.uc_mcontext.rbx;
        ctx->uc_mcontext.rbp = uc.uc_mcontext.rbp;
        ctx->uc_mcontext.r10 = uc.uc_mcontext.r10;
        ctx->uc_mcontext.r11 = uc.uc_mcontext.r11;
        ctx->uc_mcontext.r12 = uc.uc_mcontext.r12;
        ctx->uc_mcontext.r13 = uc.uc_mcontext.r13;
        ctx->uc_mcontext.r14 = uc.uc_mcontext.r14;
        ctx->uc_mcontext.r15 = uc.uc_mcontext.r15;
        ctx->uc_mcontext.trapno = uc.uc_mcontext.trapno;
        ctx->uc_mcontext.fs = uc.uc_mcontext.fs;
        ctx->uc_mcontext.gs = uc.uc_mcontext.gs;
        ctx->uc_mcontext.err = uc.uc_mcontext.err;
        ctx->uc_mcontext.rip = uc.uc_mcontext.rip;
        ctx->uc_mcontext.rsp = uc.uc_mcontext.rsp;
        __repmovsb(&ctx->uc_mcontext.__fpregs, uc.uc_mcontext.fpregs,
                   sizeof(ctx->uc_mcontext.__fpregs));
      }

    } else {
      // handle siglongjmp() call
      intptr_t *jb = tib->tib_sigjmpbuf;
      ctx->uc_mcontext.rax = jbval;
      ctx->uc_mcontext.rsp = jb[0];
      ctx->uc_mcontext.rbx = jb[1];
      ctx->uc_mcontext.rbp = jb[2];
      ctx->uc_mcontext.r12 = jb[3];
      ctx->uc_mcontext.r13 = jb[4];
      ctx->uc_mcontext.r14 = jb[5];
      ctx->uc_mcontext.r15 = jb[6];
      ctx->uc_mcontext.rip = jb[7];
    }
    tib->tib_sigjmpbuf = 0;
  }
  /*
   * When the NetBSD kernel invokes this signal handler it pushes a
   * trampoline on the stack which does two things: 1) it calls this
   * function, and 2) calls sys_sigreturn() once this returns.
   */
}

#endif /* __x86_64__ */
