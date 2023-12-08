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
#include "ape/sections.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/metasigaltstack.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo-meta.internal.h"
#include "libc/calls/struct/siginfo-xnu.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/intrin/repstosb.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"

/**
 * @fileoverview XNU kernel callback normalization.
 */

struct __darwin_mmst_reg {
  char __mmst_reg[10];
  char __mmst_rsrv[6];
};

struct __darwin_xmm_reg {
  char __xmm_reg[16];
};

struct __darwin_ymm_reg {
  char __ymm_reg[32];
};

struct __darwin_zmm_reg {
  char __zmm_reg[64];
};

struct __darwin_opmask_reg {
  char __opmask_reg[8];
};

struct __darwin_x86_thread_state64 {
  uint64_t __rax;
  uint64_t __rbx;
  uint64_t __rcx;
  uint64_t __rdx;
  uint64_t __rdi;
  uint64_t __rsi;
  uint64_t __rbp;
  uint64_t __rsp;
  uint64_t __r8;
  uint64_t __r9;
  uint64_t __r10;
  uint64_t __r11;
  uint64_t __r12;
  uint64_t __r13;
  uint64_t __r14;
  uint64_t __r15;
  uint64_t __rip;
  uint64_t __rflags;
  uint64_t __cs;
  uint64_t __fs;
  uint64_t __gs;
};

struct __darwin_x86_thread_full_state64 {
  struct __darwin_x86_thread_state64 ss64;
  uint64_t __ds;
  uint64_t __es;
  uint64_t __ss;
  uint64_t __gsbase;
};

struct __darwin_x86_float_state64 {
  int32_t __fpu_reserved[2];
  uint16_t __fpu_fcw;
  uint16_t __fpu_fsw;
  uint8_t __fpu_ftw;
  uint8_t __fpu_rsrv1;
  uint16_t __fpu_fop;
  uint32_t __fpu_ip;
  uint16_t __fpu_cs;
  uint16_t __fpu_rsrv2;
  uint32_t __fpu_dp;
  uint16_t __fpu_ds;
  uint16_t __fpu_rsrv3;
  uint32_t __fpu_mxcsr;
  uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  struct __darwin_xmm_reg __fpu_xmm8;
  struct __darwin_xmm_reg __fpu_xmm9;
  struct __darwin_xmm_reg __fpu_xmm10;
  struct __darwin_xmm_reg __fpu_xmm11;
  struct __darwin_xmm_reg __fpu_xmm12;
  struct __darwin_xmm_reg __fpu_xmm13;
  struct __darwin_xmm_reg __fpu_xmm14;
  struct __darwin_xmm_reg __fpu_xmm15;
  char __fpu_rsrv4[96];
  int32_t __fpu_reserved1;
};

struct __darwin_x86_avx_state64 {
  int32_t __fpu_reserved[2];
  uint16_t __fpu_fcw;
  uint16_t __fpu_fsw;
  uint8_t __fpu_ftw;
  uint8_t __fpu_rsrv1;
  uint16_t __fpu_fop;
  uint32_t __fpu_ip;
  uint16_t __fpu_cs;
  uint16_t __fpu_rsrv2;
  uint32_t __fpu_dp;
  uint16_t __fpu_ds;
  uint16_t __fpu_rsrv3;
  uint32_t __fpu_mxcsr;
  uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  struct __darwin_xmm_reg __fpu_xmm8;
  struct __darwin_xmm_reg __fpu_xmm9;
  struct __darwin_xmm_reg __fpu_xmm10;
  struct __darwin_xmm_reg __fpu_xmm11;
  struct __darwin_xmm_reg __fpu_xmm12;
  struct __darwin_xmm_reg __fpu_xmm13;
  struct __darwin_xmm_reg __fpu_xmm14;
  struct __darwin_xmm_reg __fpu_xmm15;
  char __fpu_rsrv4[6 * 16];
  int32_t __fpu_reserved1;
  char __avx_reserved1[64];
  struct __darwin_xmm_reg __fpu_ymmh0;
  struct __darwin_xmm_reg __fpu_ymmh1;
  struct __darwin_xmm_reg __fpu_ymmh2;
  struct __darwin_xmm_reg __fpu_ymmh3;
  struct __darwin_xmm_reg __fpu_ymmh4;
  struct __darwin_xmm_reg __fpu_ymmh5;
  struct __darwin_xmm_reg __fpu_ymmh6;
  struct __darwin_xmm_reg __fpu_ymmh7;
  struct __darwin_xmm_reg __fpu_ymmh8;
  struct __darwin_xmm_reg __fpu_ymmh9;
  struct __darwin_xmm_reg __fpu_ymmh10;
  struct __darwin_xmm_reg __fpu_ymmh11;
  struct __darwin_xmm_reg __fpu_ymmh12;
  struct __darwin_xmm_reg __fpu_ymmh13;
  struct __darwin_xmm_reg __fpu_ymmh14;
  struct __darwin_xmm_reg __fpu_ymmh15;
};

struct __darwin_x86_avx512_state64 {
  int32_t __fpu_reserved[2];
  uint16_t __fpu_fcw;
  uint16_t __fpu_fsw;
  uint8_t __fpu_ftw;
  uint8_t __fpu_rsrv1;
  uint16_t __fpu_fop;
  uint32_t __fpu_ip;
  uint16_t __fpu_cs;
  uint16_t __fpu_rsrv2;
  uint32_t __fpu_dp;
  uint16_t __fpu_ds;
  uint16_t __fpu_rsrv3;
  uint32_t __fpu_mxcsr;
  uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  struct __darwin_xmm_reg __fpu_xmm8;
  struct __darwin_xmm_reg __fpu_xmm9;
  struct __darwin_xmm_reg __fpu_xmm10;
  struct __darwin_xmm_reg __fpu_xmm11;
  struct __darwin_xmm_reg __fpu_xmm12;
  struct __darwin_xmm_reg __fpu_xmm13;
  struct __darwin_xmm_reg __fpu_xmm14;
  struct __darwin_xmm_reg __fpu_xmm15;
  char __fpu_rsrv4[6 * 16];
  int32_t __fpu_reserved1;
  char __avx_reserved1[64];
  struct __darwin_xmm_reg __fpu_ymmh0;
  struct __darwin_xmm_reg __fpu_ymmh1;
  struct __darwin_xmm_reg __fpu_ymmh2;
  struct __darwin_xmm_reg __fpu_ymmh3;
  struct __darwin_xmm_reg __fpu_ymmh4;
  struct __darwin_xmm_reg __fpu_ymmh5;
  struct __darwin_xmm_reg __fpu_ymmh6;
  struct __darwin_xmm_reg __fpu_ymmh7;
  struct __darwin_xmm_reg __fpu_ymmh8;
  struct __darwin_xmm_reg __fpu_ymmh9;
  struct __darwin_xmm_reg __fpu_ymmh10;
  struct __darwin_xmm_reg __fpu_ymmh11;
  struct __darwin_xmm_reg __fpu_ymmh12;
  struct __darwin_xmm_reg __fpu_ymmh13;
  struct __darwin_xmm_reg __fpu_ymmh14;
  struct __darwin_xmm_reg __fpu_ymmh15;
  struct __darwin_opmask_reg __fpu_k0;
  struct __darwin_opmask_reg __fpu_k1;
  struct __darwin_opmask_reg __fpu_k2;
  struct __darwin_opmask_reg __fpu_k3;
  struct __darwin_opmask_reg __fpu_k4;
  struct __darwin_opmask_reg __fpu_k5;
  struct __darwin_opmask_reg __fpu_k6;
  struct __darwin_opmask_reg __fpu_k7;
  struct __darwin_ymm_reg __fpu_zmmh0;
  struct __darwin_ymm_reg __fpu_zmmh1;
  struct __darwin_ymm_reg __fpu_zmmh2;
  struct __darwin_ymm_reg __fpu_zmmh3;
  struct __darwin_ymm_reg __fpu_zmmh4;
  struct __darwin_ymm_reg __fpu_zmmh5;
  struct __darwin_ymm_reg __fpu_zmmh6;
  struct __darwin_ymm_reg __fpu_zmmh7;
  struct __darwin_ymm_reg __fpu_zmmh8;
  struct __darwin_ymm_reg __fpu_zmmh9;
  struct __darwin_ymm_reg __fpu_zmmh10;
  struct __darwin_ymm_reg __fpu_zmmh11;
  struct __darwin_ymm_reg __fpu_zmmh12;
  struct __darwin_ymm_reg __fpu_zmmh13;
  struct __darwin_ymm_reg __fpu_zmmh14;
  struct __darwin_ymm_reg __fpu_zmmh15;
  struct __darwin_zmm_reg __fpu_zmm16;
  struct __darwin_zmm_reg __fpu_zmm17;
  struct __darwin_zmm_reg __fpu_zmm18;
  struct __darwin_zmm_reg __fpu_zmm19;
  struct __darwin_zmm_reg __fpu_zmm20;
  struct __darwin_zmm_reg __fpu_zmm21;
  struct __darwin_zmm_reg __fpu_zmm22;
  struct __darwin_zmm_reg __fpu_zmm23;
  struct __darwin_zmm_reg __fpu_zmm24;
  struct __darwin_zmm_reg __fpu_zmm25;
  struct __darwin_zmm_reg __fpu_zmm26;
  struct __darwin_zmm_reg __fpu_zmm27;
  struct __darwin_zmm_reg __fpu_zmm28;
  struct __darwin_zmm_reg __fpu_zmm29;
  struct __darwin_zmm_reg __fpu_zmm30;
  struct __darwin_zmm_reg __fpu_zmm31;
};

struct __darwin_x86_exception_state64 {
  uint16_t __trapno;
  uint16_t __cpu;
  uint32_t __err;
  uint64_t __faultvaddr;
};

struct __darwin_x86_debug_state64 {
  uint64_t __dr0;
  uint64_t __dr1;
  uint64_t __dr2;
  uint64_t __dr3;
  uint64_t __dr4;
  uint64_t __dr5;
  uint64_t __dr6;
  uint64_t __dr7;
};

struct __darwin_x86_cpmu_state64 {
  uint64_t __ctrs[16];
};

struct __darwin_mcontext64_full {
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_full_state64 __ss;
  struct __darwin_x86_float_state64 __fs;
};

struct __darwin_mcontext_avx64 {
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_avx_state64 __fs;
};

struct __darwin_mcontext_avx64_full {
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_full_state64 __ss;
  struct __darwin_x86_avx_state64 __fs;
};

struct __darwin_mcontext_avx512_64 {
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_avx512_state64 __fs;
};

struct __darwin_mcontext_avx512_64_full {
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_full_state64 __ss;
  struct __darwin_x86_avx512_state64 __fs;
};

struct __darwin_arm_exception_state64 {
  uint64_t far;
  uint32_t esr;
  uint32_t exception;
};

struct __darwin_arm_thread_state64 {
  uint64_t __x[29];
  uint64_t __fp;
  uint64_t __lr;
  uint64_t __sp;
  uint64_t __pc;
  uint32_t __cpsr;
  uint32_t __pad;
};

struct __darwin_arm_vfp_state {
  uint32_t __r[64];
  uint32_t __fpscr;
};

struct __darwin_mcontext64 {
#ifdef __x86_64__
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_float_state64 __fs;
#elif defined(__aarch64__)
  struct __darwin_arm_exception_state64 __es;
  struct __darwin_arm_thread_state64 __ss;
  struct __darwin_arm_vfp_state __fs;
#endif
};

struct __darwin_ucontext {
  int32_t uc_onstack;
  uint32_t uc_sigmask;
  struct sigaltstack_bsd uc_stack;
  struct __darwin_ucontext *uc_link;
  uint64_t uc_mcsize;
  struct __darwin_mcontext64 *uc_mcontext;
};

#ifdef __x86_64__

static privileged void xnuexceptionstate2linux(
    mcontext_t *mc, struct __darwin_x86_exception_state64 *xnues) {
  mc->trapno = xnues->__trapno;
  mc->err = xnues->__err;
}

static privileged void linuxexceptionstate2xnu(
    struct __darwin_x86_exception_state64 *xnues, mcontext_t *mc) {
  xnues->__trapno = mc->trapno;
  xnues->__err = mc->err;
}

static privileged void xnuthreadstate2linux(
    mcontext_t *mc, struct __darwin_x86_thread_state64 *xnuss) {
  mc->rdi = xnuss->__rdi;
  mc->rsi = xnuss->__rsi;
  mc->rbp = xnuss->__rbp;
  mc->rbx = xnuss->__rbx;
  mc->rdx = xnuss->__rdx;
  mc->rax = xnuss->__rax;
  mc->rcx = xnuss->__rcx;
  mc->rsp = xnuss->__rsp;
  mc->rip = xnuss->__rip;
  mc->cs = xnuss->__cs;
  mc->gs = xnuss->__gs;
  mc->fs = xnuss->__fs;
  mc->eflags = xnuss->__rflags;
  mc->r8 = xnuss->__r8;
  mc->r9 = xnuss->__r9;
  mc->r10 = xnuss->__r10;
  mc->r11 = xnuss->__r11;
  mc->r12 = xnuss->__r12;
  mc->r13 = xnuss->__r13;
  mc->r14 = xnuss->__r14;
  mc->r15 = xnuss->__r15;
}

static privileged void linuxthreadstate2xnu(
    struct __darwin_x86_thread_state64 *xnuss, ucontext_t *uc, mcontext_t *mc) {
  xnuss->__rdi = mc->rdi;
  xnuss->__rsi = mc->rsi;
  xnuss->__rbp = mc->rbp;
  xnuss->__rbx = mc->rbx;
  xnuss->__rdx = mc->rdx;
  xnuss->__rax = mc->rax;
  xnuss->__rcx = mc->rcx;
  xnuss->__rsp = mc->rsp;
  xnuss->__rip = mc->rip;
  xnuss->__cs = mc->cs;
  xnuss->__gs = mc->gs;
  xnuss->__fs = mc->fs;
  xnuss->__rflags = mc->eflags;
  xnuss->__r8 = mc->r8;
  xnuss->__r9 = mc->r9;
  xnuss->__r10 = mc->r10;
  xnuss->__r11 = mc->r11;
  xnuss->__r12 = mc->r12;
  xnuss->__r13 = mc->r13;
  xnuss->__r14 = mc->r14;
  xnuss->__r15 = mc->r15;
}

static privileged void CopyFpXmmRegs(void *d, const void *s) {
  size_t i;
  for (i = 0; i < (8 + 16) * 16; i += 16) {
    __memcpy((char *)d + i, (const char *)s + i, 16);
  }
}

static privileged void xnussefpustate2linux(
    struct FpuState *fs, struct __darwin_x86_float_state64 *xnufs) {
  fs->cwd = xnufs->__fpu_fcw;
  fs->swd = xnufs->__fpu_fsw;
  fs->ftw = xnufs->__fpu_ftw;
  fs->fop = xnufs->__fpu_fop;
  fs->rip = xnufs->__fpu_ip;
  fs->rdp = xnufs->__fpu_dp;
  fs->mxcsr = xnufs->__fpu_mxcsr;
  fs->mxcr_mask = xnufs->__fpu_mxcsrmask;
  CopyFpXmmRegs(fs->st, &xnufs->__fpu_stmm0);
}

static privileged void linuxssefpustate2xnu(
    struct __darwin_x86_float_state64 *xnufs, struct FpuState *fs) {
  xnufs->__fpu_fcw = fs->cwd;
  xnufs->__fpu_fsw = fs->swd;
  xnufs->__fpu_ftw = fs->ftw;
  xnufs->__fpu_fop = fs->fop;
  xnufs->__fpu_ip = fs->rip;
  xnufs->__fpu_dp = fs->rdp;
  xnufs->__fpu_mxcsr = fs->mxcsr;
  xnufs->__fpu_mxcsrmask = fs->mxcr_mask;
  CopyFpXmmRegs(&xnufs->__fpu_stmm0, fs->st);
}

#endif /* __x86_64__ */

#ifdef __x86_64__
privileged void __sigenter_xnu(void *fn, int infostyle, int sig,
                               struct siginfo_xnu *xnuinfo,
                               struct __darwin_ucontext *xnuctx) {
#else
privileged void __sigenter_xnu(int sig, struct siginfo_xnu *xnuinfo,
                               struct __darwin_ucontext *xnuctx) {
#endif
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct Goodies {
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
      __repstosb(&g, 0, sizeof(g));

      if (xnuctx) {
        g.uc.uc_sigmask = xnuctx->uc_sigmask;
        g.uc.uc_stack.ss_sp = xnuctx->uc_stack.ss_sp;
        g.uc.uc_stack.ss_flags = xnuctx->uc_stack.ss_flags;
        g.uc.uc_stack.ss_size = xnuctx->uc_stack.ss_size;
#ifdef __x86_64__
        g.uc.uc_mcontext.fpregs = &g.uc.__fpustate;
        if (xnuctx->uc_mcontext) {
          if (xnuctx->uc_mcsize >=
              sizeof(struct __darwin_x86_exception_state64)) {
            xnuexceptionstate2linux(&g.uc.uc_mcontext,
                                    &xnuctx->uc_mcontext->__es);
          }
          if (xnuctx->uc_mcsize >=
              (sizeof(struct __darwin_x86_exception_state64) +
               sizeof(struct __darwin_x86_thread_state64))) {
            xnuthreadstate2linux(&g.uc.uc_mcontext, &xnuctx->uc_mcontext->__ss);
          }
          if (xnuctx->uc_mcsize >= sizeof(struct __darwin_mcontext64)) {
            xnussefpustate2linux(&g.uc.__fpustate, &xnuctx->uc_mcontext->__fs);
          }
        }
#elif defined(__aarch64__)
        if (xnuctx->uc_mcontext) {
          __memcpy(g.uc.uc_mcontext.regs, &xnuctx->uc_mcontext->__ss.__x,
                   33 * 8);
        }
#endif /* __x86_64__ */
      }

      if (xnuinfo) {
        __siginfo2cosmo(&g.si, (void *)xnuinfo);
      }
      ((sigaction_f)(__executable_start + rva))(sig, &g.si, &g.uc);

      if (xnuctx) {
        xnuctx->uc_stack.ss_sp = g.uc.uc_stack.ss_sp;
        xnuctx->uc_stack.ss_flags = g.uc.uc_stack.ss_flags;
        xnuctx->uc_stack.ss_size = g.uc.uc_stack.ss_size;
        xnuctx->uc_sigmask = g.uc.uc_sigmask;
#ifdef __x86_64__
        if (xnuctx->uc_mcontext) {
          if (xnuctx->uc_mcsize >=
              sizeof(struct __darwin_x86_exception_state64)) {
            linuxexceptionstate2xnu(&xnuctx->uc_mcontext->__es,
                                    &g.uc.uc_mcontext);
          }
          if (xnuctx->uc_mcsize >=
              (sizeof(struct __darwin_x86_exception_state64) +
               sizeof(struct __darwin_x86_thread_state64))) {
            linuxthreadstate2xnu(&xnuctx->uc_mcontext->__ss, &g.uc,
                                 &g.uc.uc_mcontext);
          }
          if (xnuctx->uc_mcsize >= sizeof(struct __darwin_mcontext64)) {
            linuxssefpustate2xnu(&xnuctx->uc_mcontext->__fs, &g.uc.__fpustate);
          }
        }
#elif defined(__aarch64__)
        if (xnuctx->uc_mcontext) {
          __memcpy(&xnuctx->uc_mcontext->__ss.__x, g.uc.uc_mcontext.regs,
                   33 * 8);
        }
#endif /* __x86_64__ */
      }
    }
  }

#ifdef __x86_64__
  intptr_t ax;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(0x20000b8 /* sigreturn */), "D"(xnuctx), "S"(infostyle)
               : "rcx", "r11", "memory", "cc");
  notpossible;
#endif /* __x86_64__ */
}
