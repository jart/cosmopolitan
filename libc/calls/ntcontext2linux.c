/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/calls/ucontext.h"
#include "libc/nt/struct/context.h"
#include "libc/str/str.h"

textwindows void ntcontext2linux(ucontext_t *ctx, const struct NtContext *cr) {
  if (!cr) return;
  ctx->uc_flags = cr->EFlags;
  ctx->uc_mcontext.rax = cr->Rax;
  ctx->uc_mcontext.rbx = cr->Rbx;
  ctx->uc_mcontext.rcx = cr->Rcx;
  ctx->uc_mcontext.rdx = cr->Rdx;
  ctx->uc_mcontext.rdi = cr->Rdi;
  ctx->uc_mcontext.rsi = cr->Rsi;
  ctx->uc_mcontext.rbp = cr->Rbp;
  ctx->uc_mcontext.rsp = cr->Rsp;
  ctx->uc_mcontext.rip = cr->Rip;
  ctx->uc_mcontext.cs = cr->SegCs;
  ctx->uc_mcontext.gs = cr->SegGs;
  ctx->uc_mcontext.fs = cr->SegFs;
  ctx->uc_mcontext.fpregs = &ctx->fpustate;
  ctx->fpustate.cwd = cr->FltSave.ControlWord;
  ctx->fpustate.swd = cr->FltSave.StatusWord;
  ctx->fpustate.mxcsr = cr->FltSave.MxCsr;
  ctx->fpustate.mxcr_mask = cr->FltSave.MxCsr_Mask;
  /* copy r8,r9,r10,r11,r12,r13,r15 */
  memcpy(&ctx->uc_mcontext.r8, &cr->R8, 8 * sizeof(int64_t));
  /* copy st0-st7 as well as xmm0-xmm15 */
  memcpy(ctx->fpustate.st, &cr->FltSave.FloatRegisters,
         sizeof(ctx->fpustate.st) + sizeof(ctx->fpustate.xmm));
  memcpy(ctx->fpustate.st, &cr->FltSave.FloatRegisters,
         sizeof(ctx->fpustate.st) + sizeof(ctx->fpustate.xmm));
}
