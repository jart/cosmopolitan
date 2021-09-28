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
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/calls/ucontext.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

union sigval_openbsd {
  int32_t sival_int;
  void *sival_ptr;
};

struct siginfo_openbsd {
  int32_t si_signo;
  int32_t si_code;
  int32_t si_errno;
  union {
    int _pad[29];
    struct {
      int32_t _pid;
      union {
        struct {
          uint32_t _uid;
          union sigval_openbsd _value;
        } _kill;
        struct {
          int64_t _utime;
          int64_t _stime;
          int32_t _status;
        } _cld;
      } _pdata;
    } _proc;
    struct {
      void *_addr;
      int32_t _trapno;
    } _fault;
  } _data;
};

struct ucontext_openbsd {
  int64_t sc_rdi;
  int64_t sc_rsi;
  int64_t sc_rdx;
  int64_t sc_rcx;
  int64_t sc_r8;
  int64_t sc_r9;
  int64_t sc_r10;
  int64_t sc_r11;
  int64_t sc_r12;
  int64_t sc_r13;
  int64_t sc_r14;
  int64_t sc_r15;
  int64_t sc_rbp;
  int64_t sc_rbx;
  int64_t sc_rax;
  int64_t sc_gs;
  int64_t sc_fs;
  int64_t sc_es;
  int64_t sc_ds;
  int64_t sc_trapno;
  int64_t sc_err;
  int64_t sc_rip;
  int64_t sc_cs;
  int64_t sc_rflags;
  int64_t sc_rsp;
  int64_t sc_ss;
  struct FpuState *sc_fpstate;
  int32_t __sc_unused;
  int32_t sc_mask;
  int64_t sc_cookie;
};

void __sigenter_openbsd(int sig, struct siginfo_openbsd *si,
                        struct ucontext_openbsd *ctx) {
  int rva;
  ucontext_t uc;
  struct siginfo si2;
  rva = __sighandrvas[sig & (NSIG - 1)];
  if (rva >= kSigactionMinRva) {
    bzero(&uc, sizeof(uc));
    bzero(&si2, sizeof(si2));
    if (si) {
      si2.si_signo = si->si_signo;
      si2.si_code = si->si_code;
      si2.si_errno = si->si_errno;
    }
    if (ctx) {
      uc.uc_mcontext.fpregs = &uc.__fpustate;
      memcpy(&uc.uc_sigmask, &ctx->sc_mask,
             MIN(sizeof(uc.uc_sigmask), sizeof(ctx->sc_mask)));
      uc.uc_mcontext.rdi = ctx->sc_rdi;
      uc.uc_mcontext.rsi = ctx->sc_rsi;
      uc.uc_mcontext.rdx = ctx->sc_rdx;
      uc.uc_mcontext.rcx = ctx->sc_rcx;
      uc.uc_mcontext.r8 = ctx->sc_r8;
      uc.uc_mcontext.r9 = ctx->sc_r9;
      uc.uc_mcontext.rax = ctx->sc_rax;
      uc.uc_mcontext.rbx = ctx->sc_rbx;
      uc.uc_mcontext.rbp = ctx->sc_rbp;
      uc.uc_mcontext.r10 = ctx->sc_r10;
      uc.uc_mcontext.r11 = ctx->sc_r11;
      uc.uc_mcontext.r12 = ctx->sc_r12;
      uc.uc_mcontext.r13 = ctx->sc_r13;
      uc.uc_mcontext.r14 = ctx->sc_r14;
      uc.uc_mcontext.r15 = ctx->sc_r15;
      uc.uc_mcontext.trapno = ctx->sc_trapno;
      uc.uc_mcontext.fs = ctx->sc_fs;
      uc.uc_mcontext.gs = ctx->sc_gs;
      uc.uc_mcontext.err = ctx->sc_err;
      uc.uc_mcontext.rip = ctx->sc_rip;
      uc.uc_mcontext.rsp = ctx->sc_rsp;
      if (ctx->sc_fpstate) {
        *uc.uc_mcontext.fpregs = *ctx->sc_fpstate;
      }
    }
    ((sigaction_f)(_base + rva))(sig, &si2, &uc);
    if (ctx) {
      ctx->sc_rdi = uc.uc_mcontext.rdi;
      ctx->sc_rsi = uc.uc_mcontext.rsi;
      ctx->sc_rdx = uc.uc_mcontext.rdx;
      ctx->sc_rcx = uc.uc_mcontext.rcx;
      ctx->sc_r8 = uc.uc_mcontext.r8;
      ctx->sc_r9 = uc.uc_mcontext.r9;
      ctx->sc_rax = uc.uc_mcontext.rax;
      ctx->sc_rbx = uc.uc_mcontext.rbx;
      ctx->sc_rbp = uc.uc_mcontext.rbp;
      ctx->sc_r10 = uc.uc_mcontext.r10;
      ctx->sc_r11 = uc.uc_mcontext.r11;
      ctx->sc_r12 = uc.uc_mcontext.r12;
      ctx->sc_r13 = uc.uc_mcontext.r13;
      ctx->sc_r14 = uc.uc_mcontext.r14;
      ctx->sc_r15 = uc.uc_mcontext.r15;
      ctx->sc_trapno = uc.uc_mcontext.trapno;
      ctx->sc_fs = uc.uc_mcontext.fs;
      ctx->sc_gs = uc.uc_mcontext.gs;
      ctx->sc_err = uc.uc_mcontext.err;
      ctx->sc_rip = uc.uc_mcontext.rip;
      ctx->sc_rsp = uc.uc_mcontext.rsp;
      if (ctx->sc_fpstate) {
        *ctx->sc_fpstate = *uc.uc_mcontext.fpregs;
      }
    }
  }
  /*
   * When the OpenBSD kernel invokes this signal handler it pushes a
   * trampoline on the stack which does two things: 1) it calls this
   * function, and 2) calls sys_sigreturn() once this returns.
   */
}
