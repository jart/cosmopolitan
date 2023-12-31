#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_FREEBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_FREEBSD_INTERNAL_H_
COSMOPOLITAN_C_START_

struct gpregs_freebsd_aarch64 {
  int64_t gp_x[30];
  int64_t gp_lr;
  int64_t gp_sp;
  int64_t gp_elr;   /* pc */
  uint64_t gp_spsr; /* pstate or cpsr */
};

struct fpregs_freebsd_aarch64 {
  uint128_t fp_q[32];
  uint32_t fp_sr;
  uint32_t fp_cr;
  int fp_flags;
  int fp_pad;
};

struct stack_freebsd {
  void *ss_sp;
  uint64_t ss_size;
  int32_t ss_flags;
};

struct mcontext_freebsd {
#ifdef __x86_64__
  int64_t mc_onstack;
  int64_t mc_rdi;
  int64_t mc_rsi;
  int64_t mc_rdx;
  int64_t mc_rcx;
  int64_t mc_r8;
  int64_t mc_r9;
  int64_t mc_rax;
  int64_t mc_rbx;
  int64_t mc_rbp;
  int64_t mc_r10;
  int64_t mc_r11;
  int64_t mc_r12;
  int64_t mc_r13;
  int64_t mc_r14;
  int64_t mc_r15;
  uint32_t mc_trapno;
  uint16_t mc_fs;
  uint16_t mc_gs;
  int64_t mc_addr;
  uint32_t mc_flags;
  uint16_t mc_es;
  uint16_t mc_ds;
  int64_t mc_err;
  int64_t mc_rip;
  int64_t mc_cs;
  int64_t mc_rflags;
  int64_t mc_rsp;
  int64_t mc_ss;
  int64_t mc_len;
  int64_t mc_fpformat;
  int64_t mc_ownedfp;
  int64_t mc_fpstate[64];
  int64_t mc_fsbase;
  int64_t mc_gsbase;
  int64_t mc_xfpustate;
  int64_t mc_xfpustate_len;
  int64_t mc_spare[4];
#elif defined(__aarch64__)
  struct gpregs_freebsd_aarch64 mc_gpregs;
  struct fpregs_freebsd_aarch64 mc_fpregs;
  int mc_flags;
#define _MC_FP_VALID 0x1 /* Set when mc_fpregs has valid data */
  int mc_pad;           /* Padding */
  uint64_t mc_spare[8]; /* Space for expansion, set to zero */
#endif
};

struct ucontext_freebsd {
  uint32_t uc_sigmask[4];
  struct mcontext_freebsd uc_mcontext;
  struct ucontext_freebsd *uc_link;
  struct stack_freebsd uc_stack;
  int32_t uc_flags;
  int32_t __spare__[4];
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_FREEBSD_INTERNAL_H_ */
