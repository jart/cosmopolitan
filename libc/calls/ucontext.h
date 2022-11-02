#ifndef COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#define COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct XmmRegister {
  uint64_t u64[2];
};

struct FpuStackEntry {
  uint16_t significand[4];
  uint16_t exponent;
  uint16_t padding[3];
};

struct thatispacked FpuState {
  uint16_t cwd;
  uint16_t swd;
  uint16_t ftw;
  uint16_t fop;
  uint64_t rip;
  uint64_t rdp;
  uint32_t mxcsr;
  uint32_t mxcr_mask;
  struct FpuStackEntry st[8];
  struct XmmRegister xmm[16];
  uint32_t __padding[24];
};

typedef uint64_t greg_t;
typedef greg_t gregset_t[23];
typedef struct FpuState *fpregset_t;

struct MachineContext {
  union {
    struct {
      uint64_t r8;
      uint64_t r9;
      uint64_t r10;
      uint64_t r11;
      uint64_t r12;
      uint64_t r13;
      uint64_t r14;
      uint64_t r15;
      uint64_t rdi;
      uint64_t rsi;
      uint64_t rbp;
      uint64_t rbx;
      uint64_t rdx;
      uint64_t rax;
      uint64_t rcx;
      uint64_t rsp;
      uint64_t rip;
      uint64_t eflags;
      uint16_t cs;
      uint16_t gs;
      uint16_t fs;
      uint16_t __pad0;
      uint64_t err;
      uint64_t trapno;
      uint64_t oldmask;
      uint64_t cr2;
    };
    gregset_t gregs;
  };
  struct FpuState *fpregs; /* zero when no fpu context */
  uint64_t __pad1[8];
};

typedef struct MachineContext mcontext_t;

struct ucontext {
  uint64_t uc_flags; /* don't use this */
  struct ucontext *uc_link;
  stack_t uc_stack;
  mcontext_t uc_mcontext; /* use this */
  sigset_t uc_sigmask;
  uint64_t __pad;
  struct FpuState __fpustate; /* for cosmo on non-linux */
};

typedef struct ucontext ucontext_t;

int getcontext(ucontext_t *);
int setcontext(const ucontext_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_ */
