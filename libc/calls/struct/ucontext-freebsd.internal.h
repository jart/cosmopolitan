#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_FREEBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_FREEBSD_INTERNAL_H_
#include "libc/calls/struct/sigaction-freebsd.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

union sigval_freebsd {
  int32_t sival_int;
  void *sival_ptr;
  int32_t sigval_int;
  void *sigval_ptr;
};

struct siginfo_freebsd {
  int32_t si_signo;
  int32_t si_errno;
  int32_t si_code;
  int32_t si_pid;
  uint32_t si_uid;
  int32_t si_status;
  void *si_addr;
  union sigval_freebsd si_value;
  union {
    struct {
      int32_t _trapno;
    } _fault;
    struct {
      int32_t _timerid;
      int32_t _overrun;
    } _timer;
    struct {
      int32_t _mqd;
    } _mesgq;
    struct {
      int64_t _band;
    } _poll;
    struct {
      int64_t __spare1__;
      int32_t __spare2__[7];
    } __spare__;
  } _reason;
};

struct stack_freebsd {
  void *ss_sp;
  uint64_t ss_size;
  int32_t ss_flags;
};

struct mcontext_freebsd {
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
};

struct ucontext_freebsd {
  struct sigset_freebsd uc_sigmask;
  struct mcontext_freebsd uc_mcontext;
  struct ucontext_freebsd *uc_link;
  struct stack_freebsd uc_stack;
  int32_t uc_flags;
  int32_t __spare__[4];
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_FREEBSD_INTERNAL_H_ */
