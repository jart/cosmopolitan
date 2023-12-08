/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/ucontext.h"
#include "libc/log/libfatal.internal.h"
#include "libc/nt/struct/context.h"
#include "libc/str/str.h"
#ifdef __x86_64__

textwindows void _ntcontext2linux(ucontext_t *ctx, const struct NtContext *cr) {
  if (!cr) return;
  ctx->uc_mcontext.eflags = cr->EFlags;
  ctx->uc_mcontext.rax = cr->Rax;
  ctx->uc_mcontext.rbx = cr->Rbx;
  ctx->uc_mcontext.rcx = cr->Rcx;
  ctx->uc_mcontext.rdx = cr->Rdx;
  ctx->uc_mcontext.rdi = cr->Rdi;
  ctx->uc_mcontext.rsi = cr->Rsi;
  ctx->uc_mcontext.rbp = cr->Rbp;
  ctx->uc_mcontext.rsp = cr->Rsp;
  ctx->uc_mcontext.rip = cr->Rip;
  ctx->uc_mcontext.r8 = cr->R8;
  ctx->uc_mcontext.r9 = cr->R9;
  ctx->uc_mcontext.r10 = cr->R10;
  ctx->uc_mcontext.r11 = cr->R11;
  ctx->uc_mcontext.r12 = cr->R12;
  ctx->uc_mcontext.r13 = cr->R13;
  ctx->uc_mcontext.r14 = cr->R14;
  ctx->uc_mcontext.r15 = cr->R15;
  ctx->uc_mcontext.cs = cr->SegCs;
  ctx->uc_mcontext.gs = cr->SegGs;
  ctx->uc_mcontext.fs = cr->SegFs;
  ctx->uc_mcontext.fpregs = &ctx->__fpustate;
  __repmovsb(&ctx->__fpustate, &cr->FltSave, sizeof(ctx->__fpustate));
}

textwindows void _ntlinux2context(struct NtContext *cr, const ucontext_t *ctx) {
  if (!cr) return;
  cr->EFlags = ctx->uc_mcontext.eflags;
  cr->Rax = ctx->uc_mcontext.rax;
  cr->Rbx = ctx->uc_mcontext.rbx;
  cr->Rcx = ctx->uc_mcontext.rcx;
  cr->Rdx = ctx->uc_mcontext.rdx;
  cr->Rdi = ctx->uc_mcontext.rdi;
  cr->Rsi = ctx->uc_mcontext.rsi;
  cr->Rbp = ctx->uc_mcontext.rbp;
  cr->Rsp = ctx->uc_mcontext.rsp;
  cr->Rip = ctx->uc_mcontext.rip;
  cr->R8 = ctx->uc_mcontext.r8;
  cr->R9 = ctx->uc_mcontext.r9;
  cr->R10 = ctx->uc_mcontext.r10;
  cr->R11 = ctx->uc_mcontext.r11;
  cr->R12 = ctx->uc_mcontext.r12;
  cr->R13 = ctx->uc_mcontext.r13;
  cr->R14 = ctx->uc_mcontext.r14;
  cr->R15 = ctx->uc_mcontext.r15;
  cr->SegCs = ctx->uc_mcontext.cs;
  cr->SegGs = ctx->uc_mcontext.gs;
  cr->SegFs = ctx->uc_mcontext.fs;
  __repmovsb(&cr->FltSave, &ctx->__fpustate, sizeof(ctx->__fpustate));
}

#endif /* __x86_64__ */
