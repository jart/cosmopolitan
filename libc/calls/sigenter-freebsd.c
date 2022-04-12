/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigaction-freebsd.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext-freebsd.internal.h"
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/calls/ucontext.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

void __sigenter_freebsd(int sig, struct siginfo_freebsd *si,
                        struct ucontext_freebsd *ctx) {
  int rva;
  ucontext_t uc;
  rva = __sighandrvas[sig & (NSIG - 1)];
  if (rva >= kSigactionMinRva) {
    bzero(&uc, sizeof(uc));
    if (ctx) {
      uc.uc_mcontext.fpregs = &uc.__fpustate;
      uc.uc_stack.ss_sp = ctx->uc_stack.ss_sp;
      uc.uc_stack.ss_size = ctx->uc_stack.ss_size;
      uc.uc_stack.ss_flags = ctx->uc_stack.ss_flags;
      uc.uc_flags = ctx->uc_flags;
      memcpy(&uc.uc_sigmask, &ctx->uc_sigmask,
             MIN(sizeof(uc.uc_sigmask), sizeof(ctx->uc_sigmask)));
      uc.uc_mcontext.r8 = ctx->uc_mcontext.mc_r8;
      uc.uc_mcontext.r9 = ctx->uc_mcontext.mc_r9;
      uc.uc_mcontext.r10 = ctx->uc_mcontext.mc_r10;
      uc.uc_mcontext.r11 = ctx->uc_mcontext.mc_r11;
      uc.uc_mcontext.r12 = ctx->uc_mcontext.mc_r12;
      uc.uc_mcontext.r13 = ctx->uc_mcontext.mc_r13;
      uc.uc_mcontext.r14 = ctx->uc_mcontext.mc_r14;
      uc.uc_mcontext.r15 = ctx->uc_mcontext.mc_r15;
      uc.uc_mcontext.rdi = ctx->uc_mcontext.mc_rdi;
      uc.uc_mcontext.rsi = ctx->uc_mcontext.mc_rsi;
      uc.uc_mcontext.rbp = ctx->uc_mcontext.mc_rbp;
      uc.uc_mcontext.rbx = ctx->uc_mcontext.mc_rbx;
      uc.uc_mcontext.rdx = ctx->uc_mcontext.mc_rdx;
      uc.uc_mcontext.rax = ctx->uc_mcontext.mc_rax;
      uc.uc_mcontext.rcx = ctx->uc_mcontext.mc_rcx;
      uc.uc_mcontext.rsp = ctx->uc_mcontext.mc_rsp;
      uc.uc_mcontext.rip = ctx->uc_mcontext.mc_rip;
      uc.uc_mcontext.eflags = ctx->uc_mcontext.mc_flags;
      uc.uc_mcontext.fs = ctx->uc_mcontext.mc_fs;
      uc.uc_mcontext.gs = ctx->uc_mcontext.mc_gs;
      uc.uc_mcontext.err = ctx->uc_mcontext.mc_err;
      uc.uc_mcontext.trapno = ctx->uc_mcontext.mc_trapno;
      memcpy(&uc.__fpustate, &ctx->uc_mcontext.mc_fpstate, 512);
    }
    ((sigaction_f)(_base + rva))(sig, (void *)si, &uc);
    if (ctx) {
      ctx->uc_stack.ss_sp = uc.uc_stack.ss_sp;
      ctx->uc_stack.ss_size = uc.uc_stack.ss_size;
      ctx->uc_stack.ss_flags = uc.uc_stack.ss_flags;
      ctx->uc_flags = uc.uc_flags;
      memcpy(&ctx->uc_sigmask, &uc.uc_sigmask,
             MIN(sizeof(uc.uc_sigmask), sizeof(ctx->uc_sigmask)));
      ctx->uc_mcontext.mc_rdi = uc.uc_mcontext.rdi;
      ctx->uc_mcontext.mc_rsi = uc.uc_mcontext.rsi;
      ctx->uc_mcontext.mc_rdx = uc.uc_mcontext.rdx;
      ctx->uc_mcontext.mc_rcx = uc.uc_mcontext.rcx;
      ctx->uc_mcontext.mc_r8 = uc.uc_mcontext.r8;
      ctx->uc_mcontext.mc_r9 = uc.uc_mcontext.r9;
      ctx->uc_mcontext.mc_rax = uc.uc_mcontext.rax;
      ctx->uc_mcontext.mc_rbx = uc.uc_mcontext.rbx;
      ctx->uc_mcontext.mc_rbp = uc.uc_mcontext.rbp;
      ctx->uc_mcontext.mc_r10 = uc.uc_mcontext.r10;
      ctx->uc_mcontext.mc_r11 = uc.uc_mcontext.r11;
      ctx->uc_mcontext.mc_r12 = uc.uc_mcontext.r12;
      ctx->uc_mcontext.mc_r13 = uc.uc_mcontext.r13;
      ctx->uc_mcontext.mc_r14 = uc.uc_mcontext.r14;
      ctx->uc_mcontext.mc_r15 = uc.uc_mcontext.r15;
      ctx->uc_mcontext.mc_trapno = uc.uc_mcontext.trapno;
      ctx->uc_mcontext.mc_fs = uc.uc_mcontext.fs;
      ctx->uc_mcontext.mc_gs = uc.uc_mcontext.gs;
      ctx->uc_mcontext.mc_flags = uc.uc_mcontext.eflags;
      ctx->uc_mcontext.mc_err = uc.uc_mcontext.err;
      ctx->uc_mcontext.mc_rip = uc.uc_mcontext.rip;
      ctx->uc_mcontext.mc_rsp = uc.uc_mcontext.rsp;
      memcpy(&ctx->uc_mcontext.mc_fpstate, &uc.__fpustate, 512);
    }
  }
  /*
   * When the FreeBSD kernel invokes this signal handler it pushes a
   * trampoline on the stack which does two things: 1) it calls this
   * function, and 2) calls sys_sigreturn() once this returns.
   */
}
