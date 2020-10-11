#ifndef COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#define COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define REG_R8      REG_R8
#define REG_R9      REG_R9
#define REG_R10     REG_R10
#define REG_R11     REG_R11
#define REG_R12     REG_R12
#define REG_R13     REG_R13
#define REG_R14     REG_R14
#define REG_R15     REG_R15
#define REG_RDI     REG_RDI
#define REG_RSI     REG_RSI
#define REG_RBP     REG_RBP
#define REG_RBX     REG_RBX
#define REG_RDX     REG_RDX
#define REG_RAX     REG_RAX
#define REG_RCX     REG_RCX
#define REG_RSP     REG_RSP
#define REG_RIP     REG_RIP
#define REG_EFL     REG_EFL
#define REG_CSGSFS  REG_CSGSFS
#define REG_ERR     REG_ERR
#define REG_TRAPNO  REG_TRAPNO
#define REG_OLDMASK REG_OLDMASK
#define REG_CR2     REG_CR2

enum GeneralRegister {
  REG_R8,
  REG_R9,
  REG_R10,
  REG_R11,
  REG_R12,
  REG_R13,
  REG_R14,
  REG_R15,
  REG_RDI,
  REG_RSI,
  REG_RBP,
  REG_RBX,
  REG_RDX,
  REG_RAX,
  REG_RCX,
  REG_RSP,
  REG_RIP,
  REG_EFL,
  REG_CSGSFS,
  REG_ERR,
  REG_TRAPNO,
  REG_OLDMASK,
  REG_CR2
};

struct XmmRegister {
  uint64_t u64[2];
};

struct FpuStackEntry {
  uint16_t significand[4];
  uint16_t exponent;
  uint16_t padding[3];
};

struct FpuState {
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
  struct FpuState *fpregs;
  uint64_t __pad1[8];
};

typedef struct MachineContext mcontext_t;

struct ucontext {
  union {
    uint64_t uc_flags;
    struct {
      unsigned cf : 1;  /* bit  0: carry flag */
      unsigned vf : 1;  /* bit  1: V flag: was 8085 signed-number overflow */
      unsigned pf : 1;  /* bit  2: parity flag */
      unsigned rf : 1;  /* bit  3: always zero [undoc] */
      unsigned af : 1;  /* bit  4: auxiliary flag */
      unsigned kf : 1;  /* bit  5: K flag = V flag ⊕ sgn(result) [undoc] */
      unsigned zf : 1;  /* bit  6: zero flag */
      unsigned sf : 1;  /* bit  7: sign flag */
      unsigned tf : 1;  /* bit  8: trap flag */
      unsigned if_ : 1; /* bit  9: interrupt enable flag */
      unsigned df : 1;  /* bit 10: direction flag */
      unsigned of : 1;  /* bit 11: overflow flag */
      unsigned pl : 2;  /* b12-13: i/o privilege level (80286+) */
      unsigned nt : 1;  /* bit 14: nested task flag (80286+) */
      unsigned pc : 1;  /* bit 15: oldskool flag */
    };
  };
  struct ucontext *uc_link;
  stack_t uc_stack;
  mcontext_t uc_mcontext;
  uint8_t uc_sigmask[128 + 16]; /* ?!? wut */
  struct FpuState fpustate;
};

typedef struct ucontext ucontext_t;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_ */
