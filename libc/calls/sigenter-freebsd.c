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
#include "libc/calls/struct/aarch64.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo-freebsd.internal.h"
#include "libc/calls/struct/siginfo-meta.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext-freebsd.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"

privileged void __sigenter_freebsd(int sig, struct siginfo_freebsd *freebsdinfo,
                                   struct ucontext_freebsd *ctx) {

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct {
    ucontext_t uc;
    siginfo_t si;
  } g;
  CheckLargeStackAllocation(&g, sizeof(g));
#pragma GCC pop_options

  int rva, flags;
  rva = __sighandrvas[sig];
  if (rva >= kSigactionMinRva) {
    flags = __sighandflags[sig];
    if (~flags & SA_SIGINFO) {
      ((sigaction_f)(__executable_start + rva))(sig, 0, 0);
    } else {

      //
      // TRANSLATE FREEBSD SIGNAL TO LINUX SIGNAL
      //

      __repstosb(&g, 0, sizeof(g));
      __siginfo2cosmo(&g.si, (void *)freebsdinfo);
      g.uc.uc_stack.ss_sp = ctx->uc_stack.ss_sp;
      g.uc.uc_stack.ss_size = ctx->uc_stack.ss_size;
      g.uc.uc_stack.ss_flags = ctx->uc_stack.ss_flags;
      g.uc.uc_sigmask = ctx->uc_sigmask[0] | (uint64_t)ctx->uc_sigmask[0] << 32;

#ifdef __x86_64__
      g.uc.uc_mcontext.fpregs = &g.uc.__fpustate;
      g.uc.uc_mcontext.r8 = ctx->uc_mcontext.mc_r8;
      g.uc.uc_mcontext.r9 = ctx->uc_mcontext.mc_r9;
      g.uc.uc_mcontext.r10 = ctx->uc_mcontext.mc_r10;
      g.uc.uc_mcontext.r11 = ctx->uc_mcontext.mc_r11;
      g.uc.uc_mcontext.r12 = ctx->uc_mcontext.mc_r12;
      g.uc.uc_mcontext.r13 = ctx->uc_mcontext.mc_r13;
      g.uc.uc_mcontext.r14 = ctx->uc_mcontext.mc_r14;
      g.uc.uc_mcontext.r15 = ctx->uc_mcontext.mc_r15;
      g.uc.uc_mcontext.rdi = ctx->uc_mcontext.mc_rdi;
      g.uc.uc_mcontext.rsi = ctx->uc_mcontext.mc_rsi;
      g.uc.uc_mcontext.rbp = ctx->uc_mcontext.mc_rbp;
      g.uc.uc_mcontext.rbx = ctx->uc_mcontext.mc_rbx;
      g.uc.uc_mcontext.rdx = ctx->uc_mcontext.mc_rdx;
      g.uc.uc_mcontext.rax = ctx->uc_mcontext.mc_rax;
      g.uc.uc_mcontext.rcx = ctx->uc_mcontext.mc_rcx;
      g.uc.uc_mcontext.rsp = ctx->uc_mcontext.mc_rsp;
      g.uc.uc_mcontext.rip = ctx->uc_mcontext.mc_rip;
      g.uc.uc_mcontext.eflags = ctx->uc_mcontext.mc_flags;
      g.uc.uc_mcontext.fs = ctx->uc_mcontext.mc_fs;
      g.uc.uc_mcontext.gs = ctx->uc_mcontext.mc_gs;
      g.uc.uc_mcontext.err = ctx->uc_mcontext.mc_err;
      g.uc.uc_mcontext.trapno = ctx->uc_mcontext.mc_trapno;
      __repmovsb(&g.uc.__fpustate, &ctx->uc_mcontext.mc_fpstate, 512);
#endif /* __x86_64__ */

#ifdef __aarch64__
      __memcpy(g.uc.uc_mcontext.regs, &ctx->uc_mcontext.mc_gpregs, 34 * 8);
      if (ctx->uc_mcontext.mc_flags & _MC_FP_VALID) {
        struct fpsimd_context *vc =
            (struct fpsimd_context *)g.uc.uc_mcontext.__reserved;
        vc->head.magic = FPSIMD_MAGIC;
        vc->head.size = sizeof(*vc);
        vc->fpsr = ctx->uc_mcontext.mc_fpregs.fp_sr;
        vc->fpcr = ctx->uc_mcontext.mc_fpregs.fp_cr;
        __memcpy(vc->vregs, ctx->uc_mcontext.mc_fpregs.fp_q, 32 * 16);
      }
#endif /* __aarch64__ */

      //
      // INVOKE SIGNAL HANDLER
      //

      ((sigaction_f)(__executable_start + rva))(sig, &g.si, &g.uc);

      //
      // TRANSLATE LINUX SIGNAL TO FREEBSD SIGNAL
      //

      ctx->uc_stack.ss_sp = g.uc.uc_stack.ss_sp;
      ctx->uc_stack.ss_size = g.uc.uc_stack.ss_size;
      ctx->uc_stack.ss_flags = g.uc.uc_stack.ss_flags;
      ctx->uc_flags = g.uc.uc_flags;
      ctx->uc_sigmask[0] = g.uc.uc_sigmask;
      ctx->uc_sigmask[1] = g.uc.uc_sigmask >> 32;

#ifdef __x86_64__
      ctx->uc_mcontext.mc_rdi = g.uc.uc_mcontext.rdi;
      ctx->uc_mcontext.mc_rsi = g.uc.uc_mcontext.rsi;
      ctx->uc_mcontext.mc_rdx = g.uc.uc_mcontext.rdx;
      ctx->uc_mcontext.mc_rcx = g.uc.uc_mcontext.rcx;
      ctx->uc_mcontext.mc_r8 = g.uc.uc_mcontext.r8;
      ctx->uc_mcontext.mc_r9 = g.uc.uc_mcontext.r9;
      ctx->uc_mcontext.mc_rax = g.uc.uc_mcontext.rax;
      ctx->uc_mcontext.mc_rbx = g.uc.uc_mcontext.rbx;
      ctx->uc_mcontext.mc_rbp = g.uc.uc_mcontext.rbp;
      ctx->uc_mcontext.mc_r10 = g.uc.uc_mcontext.r10;
      ctx->uc_mcontext.mc_r11 = g.uc.uc_mcontext.r11;
      ctx->uc_mcontext.mc_r12 = g.uc.uc_mcontext.r12;
      ctx->uc_mcontext.mc_r13 = g.uc.uc_mcontext.r13;
      ctx->uc_mcontext.mc_r14 = g.uc.uc_mcontext.r14;
      ctx->uc_mcontext.mc_r15 = g.uc.uc_mcontext.r15;
      ctx->uc_mcontext.mc_trapno = g.uc.uc_mcontext.trapno;
      ctx->uc_mcontext.mc_fs = g.uc.uc_mcontext.fs;
      ctx->uc_mcontext.mc_gs = g.uc.uc_mcontext.gs;
      ctx->uc_mcontext.mc_flags = g.uc.uc_mcontext.eflags;
      ctx->uc_mcontext.mc_err = g.uc.uc_mcontext.err;
      ctx->uc_mcontext.mc_rip = g.uc.uc_mcontext.rip;
      ctx->uc_mcontext.mc_rsp = g.uc.uc_mcontext.rsp;
      __repmovsb(&ctx->uc_mcontext.mc_fpstate, &g.uc.__fpustate, 512);
#endif /* __x86_64__ */

#ifdef __aarch64__
      __memcpy(&ctx->uc_mcontext.mc_gpregs, g.uc.uc_mcontext.regs, 34 * 8);
      struct fpsimd_context *vc =
          (struct fpsimd_context *)g.uc.uc_mcontext.__reserved;
      if (vc->head.magic == FPSIMD_MAGIC) {
        ctx->uc_mcontext.mc_flags |= _MC_FP_VALID;
        ctx->uc_mcontext.mc_fpregs.fp_sr = vc->fpsr;
        ctx->uc_mcontext.mc_fpregs.fp_cr = vc->fpcr;
        __memcpy(ctx->uc_mcontext.mc_fpregs.fp_q, vc->vregs, 32 * 16);
      }
#endif /* __aarch64__ */

      // done
    }
  }
  // When the FreeBSD kernel invokes this signal handler it pushes a
  // trampoline on the stack which does two things: 1) it calls this
  // function, and 2) calls sys_sigreturn() once this returns.
}
