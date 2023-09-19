#ifndef COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#define COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifdef __x86_64__

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

#endif /* __x86_64__ */

struct sigcontext {
#ifdef __x86_64__
  union {
    struct {
      uint64_t r8;     /* 40 */
      uint64_t r9;     /* 48 */
      uint64_t r10;    /* 56 */
      uint64_t r11;    /* 64 */
      uint64_t r12;    /* 72 */
      uint64_t r13;    /* 80 */
      uint64_t r14;    /* 88 */
      uint64_t r15;    /* 96 */
      uint64_t rdi;    /* 104 */
      uint64_t rsi;    /* 112 */
      uint64_t rbp;    /* 120 */
      uint64_t rbx;    /* 128 */
      uint64_t rdx;    /* 136 */
      uint64_t rax;    /* 144 */
      uint64_t rcx;    /* 152 */
      uint64_t rsp;    /* 160 */
      uint64_t rip;    /* 168 */
      uint64_t eflags; /* 176 */
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
#elif defined(__aarch64__)
  uint64_t fault_address;
  uint64_t regs[31];
  uint64_t sp;
  uint64_t pc;
  uint64_t pstate;
  uint8_t __reserved[4096] __attribute__((__aligned__(16)));
#endif /* __x86_64__ */
};

typedef struct sigcontext mcontext_t;

struct ucontext {
  uint64_t uc_flags; /* don't use this */
  struct ucontext *uc_link;
  stack_t uc_stack;
#ifdef __x86_64__
  struct sigcontext uc_mcontext;
  sigset_t uc_sigmask;
  uint64_t __pad;
  struct FpuState __fpustate; /* for cosmo on non-linux */
#elif defined(__aarch64__)
  sigset_t uc_sigmask;
  uint8_t __unused[1024 / 8 - sizeof(sigset_t)];
  struct sigcontext uc_mcontext;
#endif
} forcealign(16);

typedef struct ucontext ucontext_t;

int getcontext(ucontext_t *) dontthrow;
int setcontext(const ucontext_t *) dontthrow;
int swapcontext(ucontext_t *, const ucontext_t *) dontthrow returnstwice;
void makecontext(ucontext_t *, void (*)(), int, ...) dontthrow nocallback;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_ */
