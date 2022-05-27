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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction-freebsd.internal.h"
#include "libc/calls/struct/siginfo-openbsd.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext-openbsd.internal.h"
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/calls/ucontext.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"

privileged void __sigenter_openbsd(int sig, struct siginfo_openbsd *openbsdinfo,
                                   struct ucontext_openbsd *ctx) {
  int rva, flags;
  struct Goodies {
    ucontext_t uc;
    struct siginfo si;
  } g;
  rva = __sighandrvas[sig & (NSIG - 1)];
  if (rva >= kSigactionMinRva) {
    flags = __sighandflags[sig & (NSIG - 1)];
    if (~flags & SA_SIGINFO) {
      ((sigaction_f)(_base + rva))(sig, 0, 0);
    } else {
      __repstosb(&g, 0, sizeof(g));
      g.si.si_signo = openbsdinfo->si_signo;
      g.si.si_code = openbsdinfo->si_code;
      g.si.si_errno = openbsdinfo->si_errno;
      if (openbsdinfo->si_pid) {
        g.si.si_pid = openbsdinfo->si_pid;
        g.si.si_uid = openbsdinfo->si_uid;
      } else {
        g.si.si_addr = (void *)openbsdinfo->si_addr;
      }
      g.si.si_value = openbsdinfo->si_value;
      g.uc.uc_mcontext.fpregs = &g.uc.__fpustate;
      __repmovsb(&g.uc.uc_sigmask, &ctx->sc_mask,
                 MIN(sizeof(g.uc.uc_sigmask), sizeof(ctx->sc_mask)));
      g.uc.uc_mcontext.rdi = ctx->sc_rdi;
      g.uc.uc_mcontext.rsi = ctx->sc_rsi;
      g.uc.uc_mcontext.rdx = ctx->sc_rdx;
      g.uc.uc_mcontext.rcx = ctx->sc_rcx;
      g.uc.uc_mcontext.r8 = ctx->sc_r8;
      g.uc.uc_mcontext.r9 = ctx->sc_r9;
      g.uc.uc_mcontext.rax = ctx->sc_rax;
      g.uc.uc_mcontext.rbx = ctx->sc_rbx;
      g.uc.uc_mcontext.rbp = ctx->sc_rbp;
      g.uc.uc_mcontext.r10 = ctx->sc_r10;
      g.uc.uc_mcontext.r11 = ctx->sc_r11;
      g.uc.uc_mcontext.r12 = ctx->sc_r12;
      g.uc.uc_mcontext.r13 = ctx->sc_r13;
      g.uc.uc_mcontext.r14 = ctx->sc_r14;
      g.uc.uc_mcontext.r15 = ctx->sc_r15;
      g.uc.uc_mcontext.trapno = ctx->sc_trapno;
      g.uc.uc_mcontext.fs = ctx->sc_fs;
      g.uc.uc_mcontext.gs = ctx->sc_gs;
      g.uc.uc_mcontext.err = ctx->sc_err;
      g.uc.uc_mcontext.rip = ctx->sc_rip;
      g.uc.uc_mcontext.rsp = ctx->sc_rsp;
      if (ctx->sc_fpstate) {
        *g.uc.uc_mcontext.fpregs = *ctx->sc_fpstate;
      }
      ((sigaction_f)(_base + rva))(sig, &g.si, &g.uc);
      ctx->sc_rdi = g.uc.uc_mcontext.rdi;
      ctx->sc_rsi = g.uc.uc_mcontext.rsi;
      ctx->sc_rdx = g.uc.uc_mcontext.rdx;
      ctx->sc_rcx = g.uc.uc_mcontext.rcx;
      ctx->sc_r8 = g.uc.uc_mcontext.r8;
      ctx->sc_r9 = g.uc.uc_mcontext.r9;
      ctx->sc_rax = g.uc.uc_mcontext.rax;
      ctx->sc_rbx = g.uc.uc_mcontext.rbx;
      ctx->sc_rbp = g.uc.uc_mcontext.rbp;
      ctx->sc_r10 = g.uc.uc_mcontext.r10;
      ctx->sc_r11 = g.uc.uc_mcontext.r11;
      ctx->sc_r12 = g.uc.uc_mcontext.r12;
      ctx->sc_r13 = g.uc.uc_mcontext.r13;
      ctx->sc_r14 = g.uc.uc_mcontext.r14;
      ctx->sc_r15 = g.uc.uc_mcontext.r15;
      ctx->sc_trapno = g.uc.uc_mcontext.trapno;
      ctx->sc_fs = g.uc.uc_mcontext.fs;
      ctx->sc_gs = g.uc.uc_mcontext.gs;
      ctx->sc_err = g.uc.uc_mcontext.err;
      ctx->sc_rip = g.uc.uc_mcontext.rip;
      ctx->sc_rsp = g.uc.uc_mcontext.rsp;
      if (ctx->sc_fpstate) {
        *ctx->sc_fpstate = *g.uc.uc_mcontext.fpregs;
      }
    }
  }
  /*
   * When the OpenBSD kernel invokes this signal handler it pushes a
   * trampoline on the stack which does two things: 1) it calls this
   * function, and 2) calls sys_sigreturn() once this returns.
   */
}
