/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/str/str.h"

/**
 * @fileoverview XNU kernel callback normalization.
 */

union __darwin_sigval {
  int32_t sival_int;
  void *sival_ptr;
};

struct __darwin_siginfo {
  int32_t si_signo;
  int32_t si_errno;
  int32_t si_code;
  int32_t si_pid;
  uint32_t si_uid;
  int32_t si_status;
  void *si_addr;
  union __darwin_sigval si_value;
  int64_t si_band;
  uint64_t __pad[7];
};

struct __darwin_sigaltstack {
  void *ss_sp;
  uint64_t ss_size;
  int32_t ss_flags;
};

struct __darwin_fp_control {
  uint16_t __invalid : 1, __denorm : 1, __zdiv : 1, __ovrfl : 1, __undfl : 1,
      __precis : 1, : 2, __pc : 2, __rc : 2, : 1, : 3;
};

struct __darwin_fp_status {
  uint16_t __invalid : 1, __denorm : 1, __zdiv : 1, __ovrfl : 1, __undfl : 1,
      __precis : 1, __stkflt : 1, __errsumm : 1, __c0 : 1, __c1 : 1, __c2 : 1,
      __tos : 3, __c3 : 1, __busy : 1;
};

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
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
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
};

struct __darwin_x86_avx_state64 {
  int32_t __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
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
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
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

struct __darwin_mcontext64 {
  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_float_state64 __fs;
};

struct __darwin_ucontext {
  int32_t uc_onstack;
  uint32_t uc_sigmask;
  struct __darwin_sigaltstack uc_stack;
  struct __darwin_ucontext *uc_link;
  uint64_t uc_mcsize;
  struct __darwin_mcontext64 *uc_mcontext;
};

static void xnuexceptionstate2linux(
    mcontext_t *mc, struct __darwin_x86_exception_state64 *xnues) {
  mc->trapno = xnues->__trapno;
  mc->err = xnues->__err;
}

static void xnuthreadstate2linux(ucontext_t *uc, mcontext_t *mc,
                                 struct __darwin_x86_thread_state64 *xnuss) {
  mc->rdi = xnuss->__rdi;
  mc->rsi = xnuss->__rsi;
  mc->rbp = xnuss->__rbp;
  mc->rbx = xnuss->__rbx;
  mc->rdx = xnuss->__rdx;
  mc->rax = xnuss->__rax;
  mc->rcx = xnuss->__rcx;
  mc->rsp = xnuss->__rsp;
  mc->rip = xnuss->__rip;
  /* g.uc.uc_mcontext.rip = xnuctx->uc_mcontext->__es.__faultvaddr; */
  mc->cs = xnuss->__cs;
  mc->gs = xnuss->__gs;
  mc->fs = xnuss->__fs;
  mc->eflags = xnuss->__rflags;
  uc->uc_flags = xnuss->__rflags;
  memcpy(&mc->r8, &xnuss->__r8, 8 * sizeof(int64_t));
}

static void xnussefpustate2linux(struct FpuState *fs,
                                 struct __darwin_x86_float_state64 *xnufs) {
  memcpy(&fs->cwd, &xnufs->__fpu_fcw, 2);
  memcpy(&fs->swd, &xnufs->__fpu_fsw, 2);
  fs->ftw = xnufs->__fpu_ftw;
  fs->fop = xnufs->__fpu_fop;
  fs->rip = xnufs->__fpu_ip;
  fs->rdp = xnufs->__fpu_dp;
  fs->mxcsr = xnufs->__fpu_mxcsr;
  fs->mxcr_mask = xnufs->__fpu_mxcsrmask;
  /* copy st0-st7 as well as xmm0-xmm15 */
  memcpy(fs->st, &xnufs->__fpu_stmm0, (8 + 16) * sizeof(uint128_t));
}

wontreturn void xnutrampoline(void *fn, int infostyle, int sig,
                              const struct __darwin_siginfo *xnuinfo,
                              const struct __darwin_ucontext *xnuctx) {
  /* note: this function impl can't access static memory */
  intptr_t ax;
  struct Goodies {
    ucontext_t uc;
    siginfo_t si;
  } g;
  memset(&g, 0, sizeof(g));
  if (xnuctx) {
    /* g.uc.uc_sigmask = xnuctx->uc_sigmask; */
    g.uc.uc_stack.ss_sp = xnuctx->uc_stack.ss_sp;
    g.uc.uc_stack.ss_flags = xnuctx->uc_stack.ss_flags;
    g.uc.uc_stack.ss_size = xnuctx->uc_stack.ss_size;
    if (xnuctx->uc_mcontext) {
      if (xnuctx->uc_mcsize >= sizeof(struct __darwin_x86_exception_state64)) {
        xnuexceptionstate2linux(&g.uc.uc_mcontext, &xnuctx->uc_mcontext->__es);
      }
      if (xnuctx->uc_mcsize >= (sizeof(struct __darwin_x86_exception_state64) +
                                sizeof(struct __darwin_x86_thread_state64))) {
        xnuthreadstate2linux(&g.uc, &g.uc.uc_mcontext,
                             &xnuctx->uc_mcontext->__ss);
      }
      if (xnuctx->uc_mcsize >= sizeof(struct __darwin_mcontext64)) {
        xnussefpustate2linux(&g.uc.fpustate, &xnuctx->uc_mcontext->__fs);
      }
    }
  }
  if (xnuinfo) {
    g.si.si_signo = xnuinfo->si_signo;
    g.si.si_errno = xnuinfo->si_errno;
    g.si.si_code = xnuinfo->si_code;
    if (xnuinfo->si_pid) {
      g.si.si_pid = xnuinfo->si_pid;
      g.si.si_uid = xnuinfo->si_uid;
      g.si.si_status = xnuinfo->si_status;
    } else {
      g.si.si_addr = (void *)xnuinfo->si_addr;
    }
  }
  __sigenter(sig, &g.si, &g.uc);
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(0x20000b8 /* sigreturn */), "D"(xnuctx), "S"(infostyle)
               : "rcx", "r11", "memory", "cc");
  unreachable;
}
