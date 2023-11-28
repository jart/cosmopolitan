#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_NETBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_NETBSD_INTERNAL_H_
#include "libc/calls/ucontext.h"
COSMOPOLITAN_C_START_
// clang-format off

#ifdef __x86_64__

#define __UCONTEXT_SIZE 784
#define _UC_SIGMASK     0x01
#define _UC_STACK       0x02
#define _UC_CPU         0x04
#define _UC_FPU         0x08
#define _UC_TLSBASE     0x00080000
#define _UC_SETSTACK    0x00010000
#define _UC_CLRSTACK    0x00020000

union sigval_netbsd {
  int32_t sival_int;
  void *sival_ptr;
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
  union {
    struct {
      uint64_t rdi;
      uint64_t rsi;
      uint64_t rdx;
      uint64_t rcx;
      uint64_t r8;
      uint64_t r9;
      uint64_t r10;
      uint64_t r11;
      uint64_t r12;
      uint64_t r13;
      uint64_t r14;
      uint64_t r15;
      uint64_t rbp;
      uint64_t rbx;
      uint64_t rax;
      uint64_t gs;
      uint64_t fs;
      uint64_t es;
      uint64_t ds;
      uint64_t trapno;
      uint64_t err;
      uint64_t rip;
      uint64_t cs;
      uint64_t rflags;
      uint64_t rsp;
      uint64_t ss;
    };
    int64_t __gregs[26];
  };
  int64_t _mc_tlsbase;
  struct FpuState __fpregs;
};

struct ucontext_netbsd {
  union {
    struct {
      uint32_t uc_flags; /* see _UC_* above */
      struct ucontext_netbsd *uc_link;
      struct sigset_netbsd uc_sigmask;
      struct stack_netbsd uc_stack;
      struct mcontext_netbsd uc_mcontext;
    };
    char __pad[__UCONTEXT_SIZE];
  };
};

#endif /* __x86_64__ */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_NETBSD_INTERNAL_H_ */
