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

#define RDI    0
#define RSI    1
#define RDX    2
#define R10    6
#define R8     4
#define R9     5
#define RCX    3
#define R11    7
#define R12    8
#define R13    9
#define R14    10
#define R15    11
#define RBP    12
#define RBX    13
#define RAX    14
#define GS     15
#define FS     16
#define ES     17
#define DS     18
#define TRAP   19
#define ERR    20
#define RIP    21
#define CS     22
#define RFLAGS 23
#define RSP    24
#define SS     25

union sigval_netbsd {
  int32_t sival_int;
  void *sival_ptr;
};

struct siginfo_netbsd {
  int32_t _signo;
  int32_t _code;
  int32_t _errno;
  int32_t _pad;
  union {
    struct {
      int32_t _pid;
      uint32_t _uid;
      union sigval_netbsd _value;
    } _rt;
    struct {
      int32_t _pid;
      uint32_t _uid;
      int32_t _status;
      int64_t _utime;
      int64_t _stime;
    } _child;
    struct {
      void *_addr;
      int32_t _trap;
      int32_t _trap2;
      int32_t _trap3;
    } _fault;
    struct {
      int64_t _band;
      int32_t _fd;
    } _poll;
    struct {
      int32_t _sysnum;
      int32_t _retval[2];
      int32_t _error;
      uint64_t _args[8];
    } _syscall;
    struct {
      int32_t _pe_report_event;
      union {
        int32_t _pe_other_pid;
        int32_t _pe_lwp;
      } _option;
    } _ptrace_state;
  } _reason;
};

struct sigset_netbsd {
  uint32_t __bits[4];
};

struct stack_netbsd {
  void *ss_sp;
  size_t ss_size;
  int32_t ss_flags;
};

struct mcontext_netbsd {
  int64_t __gregs[26];
  int64_t _mc_tlsbase;
  struct FpuState __fpregs;
};

struct ucontext_netbsd {
  uint32_t uc_flags;
  struct ucontext_netbsd *uc_link;
  struct sigset_netbsd uc_sigmask;
  struct stack_netbsd uc_stack;
  struct mcontext_netbsd uc_mcontext;
};

void __sigenter_netbsd(int sig, struct siginfo_netbsd *si,
                       struct ucontext_netbsd *ctx) {
  int rva;
  ucontext_t uc;
  struct siginfo si2;
  rva = __sighandrvas[sig & (NSIG - 1)];
  if (rva >= kSigactionMinRva) {
    bzero(&uc, sizeof(uc));
    bzero(&si2, sizeof(si2));
    if (si) {
      si2.si_signo = si->_signo;
      si2.si_code = si->_code;
      si2.si_errno = si->_errno;
    }
    if (ctx) {
      uc.uc_mcontext.fpregs = &uc.__fpustate;
      uc.uc_flags = ctx->uc_flags;
      uc.uc_stack.ss_sp = ctx->uc_stack.ss_sp;
      uc.uc_stack.ss_size = ctx->uc_stack.ss_size;
      uc.uc_stack.ss_flags = ctx->uc_stack.ss_flags;
      memcpy(&uc.uc_sigmask, &ctx->uc_sigmask,
             MIN(sizeof(uc.uc_sigmask), sizeof(ctx->uc_sigmask)));
      uc.uc_mcontext.rdi = ctx->uc_mcontext.__gregs[RDI];
      uc.uc_mcontext.rsi = ctx->uc_mcontext.__gregs[RSI];
      uc.uc_mcontext.rdx = ctx->uc_mcontext.__gregs[RDX];
      uc.uc_mcontext.rcx = ctx->uc_mcontext.__gregs[RCX];
      uc.uc_mcontext.r8 = ctx->uc_mcontext.__gregs[R8];
      uc.uc_mcontext.r9 = ctx->uc_mcontext.__gregs[R9];
      uc.uc_mcontext.rax = ctx->uc_mcontext.__gregs[RAX];
      uc.uc_mcontext.rbx = ctx->uc_mcontext.__gregs[RBX];
      uc.uc_mcontext.rbp = ctx->uc_mcontext.__gregs[RBP];
      uc.uc_mcontext.r10 = ctx->uc_mcontext.__gregs[R10];
      uc.uc_mcontext.r11 = ctx->uc_mcontext.__gregs[R11];
      uc.uc_mcontext.r12 = ctx->uc_mcontext.__gregs[R12];
      uc.uc_mcontext.r13 = ctx->uc_mcontext.__gregs[R13];
      uc.uc_mcontext.r14 = ctx->uc_mcontext.__gregs[R14];
      uc.uc_mcontext.r15 = ctx->uc_mcontext.__gregs[R15];
      uc.uc_mcontext.trapno = ctx->uc_mcontext.__gregs[TRAP];
      uc.uc_mcontext.fs = ctx->uc_mcontext.__gregs[FS];
      uc.uc_mcontext.gs = ctx->uc_mcontext.__gregs[GS];
      uc.uc_mcontext.err = ctx->uc_mcontext.__gregs[ERR];
      uc.uc_mcontext.rip = ctx->uc_mcontext.__gregs[RIP];
      uc.uc_mcontext.rsp = ctx->uc_mcontext.__gregs[RSP];
      *uc.uc_mcontext.fpregs = ctx->uc_mcontext.__fpregs;
    }
    ((sigaction_f)(_base + rva))(sig, &si2, &uc);
    if (ctx) {
      ctx->uc_mcontext.__gregs[RDI] = uc.uc_mcontext.rdi;
      ctx->uc_mcontext.__gregs[RSI] = uc.uc_mcontext.rsi;
      ctx->uc_mcontext.__gregs[RDX] = uc.uc_mcontext.rdx;
      ctx->uc_mcontext.__gregs[RCX] = uc.uc_mcontext.rcx;
      ctx->uc_mcontext.__gregs[R8] = uc.uc_mcontext.r8;
      ctx->uc_mcontext.__gregs[R9] = uc.uc_mcontext.r9;
      ctx->uc_mcontext.__gregs[RAX] = uc.uc_mcontext.rax;
      ctx->uc_mcontext.__gregs[RBX] = uc.uc_mcontext.rbx;
      ctx->uc_mcontext.__gregs[RBP] = uc.uc_mcontext.rbp;
      ctx->uc_mcontext.__gregs[R10] = uc.uc_mcontext.r10;
      ctx->uc_mcontext.__gregs[R11] = uc.uc_mcontext.r11;
      ctx->uc_mcontext.__gregs[R12] = uc.uc_mcontext.r12;
      ctx->uc_mcontext.__gregs[R13] = uc.uc_mcontext.r13;
      ctx->uc_mcontext.__gregs[R14] = uc.uc_mcontext.r14;
      ctx->uc_mcontext.__gregs[R15] = uc.uc_mcontext.r15;
      ctx->uc_mcontext.__gregs[TRAP] = uc.uc_mcontext.trapno;
      ctx->uc_mcontext.__gregs[FS] = uc.uc_mcontext.fs;
      ctx->uc_mcontext.__gregs[GS] = uc.uc_mcontext.gs;
      ctx->uc_mcontext.__gregs[ERR] = uc.uc_mcontext.err;
      ctx->uc_mcontext.__gregs[RIP] = uc.uc_mcontext.rip;
      ctx->uc_mcontext.__gregs[RSP] = uc.uc_mcontext.rsp;
      ctx->uc_mcontext.__fpregs = *uc.uc_mcontext.fpregs;
    }
  }
  /*
   * When the NetBSD kernel invokes this signal handler it pushes a
   * trampoline on the stack which does two things: 1) it calls this
   * function, and 2) calls sys_sigreturn() once this returns.
   */
}
