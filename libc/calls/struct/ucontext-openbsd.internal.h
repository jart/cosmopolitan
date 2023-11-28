#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_OPENBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_OPENBSD_INTERNAL_H_
#include "libc/calls/ucontext.h"
COSMOPOLITAN_C_START_

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
  uint32_t sc_mask;
  int64_t sc_cookie;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_OPENBSD_INTERNAL_H_ */
