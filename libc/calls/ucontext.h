#ifndef COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#define COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
COSMOPOLITAN_C_START_

#ifdef __x86_64__

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
#define REG_R8      0
#define REG_R9      1
#define REG_R10     2
#define REG_R11     3
#define REG_R12     4
#define REG_R13     5
#define REG_R14     6
#define REG_R15     7
#define REG_RDI     8
#define REG_RSI     9
#define REG_RBP     10
#define REG_RBX     11
#define REG_RDX     12
#define REG_RAX     13
#define REG_RCX     14
#define REG_RSP     15
#define REG_RIP     16
#define REG_EFL     17
#define REG_CSGSFS  18
#define REG_ERR     19
#define REG_TRAPNO  20
#define REG_OLDMASK 21
#define REG_CR2     22
#endif

struct XmmRegister {
  uint64_t u64[2];
};

struct FpuStackEntry {
  uint16_t significand[4];
  uint16_t exponent;
  uint16_t padding[3];
};

struct thatispacked FpuState {

  /* 8087 FPU Control Word
      IM: Invalid Operation ───────────────┐
      DM: Denormal Operand ───────────────┐│
      ZM: Zero Divide ───────────────────┐││
      OM: Overflow ─────────────────────┐│││
      UM: Underflow ───────────────────┐││││
      PM: Precision ──────────────────┐│││││
      PC: Precision Control ───────┐  ││││││
       {float,∅,double,long double}│  ││││││
      RC: Rounding Control ──────┐ │  ││││││
       {even, →-∞, →+∞, →0}      │┌┤  ││││││
                                ┌┤││  ││││││
                               d││││rr││││││
                          0b0000001001111111 */
  uint16_t cwd;

  /* 8087 FPU Status Word */
  uint16_t swd;

  uint16_t ftw;
  uint16_t fop;
  uint64_t rip;
  uint64_t rdp;

  /* SSE CONTROL AND STATUS REGISTER
     IE: Invalid Operation Flag ──────────────┐
     DE: Denormal Flag ──────────────────────┐│
     ZE: Divide-by-Zero Flag ───────────────┐││
     OE: Overflow Flag ────────────────────┐│││
     UE: Underflow Flag ──────────────────┐││││
     PE: Precision Flag ─────────────────┐│││││
     DAZ: Denormals Are Zeros ──────────┐││││││
     IM: Invalid Operation Mask ───────┐│││││││
     DM: Denormal Operation Mask ─────┐││││││││
     ZM: Divide-by-Zero Mask ────────┐│││││││││
     OM: Overflow Mask ─────────────┐││││││││││
     UM: Underflow Mask ───────────┐│││││││││││
     PM: Precision Mask ──────────┐││││││││││││
     RC: Rounding Control ───────┐│││││││││││││
       {even, →-∞, →+∞, →0}      ││││││││││││││
     FTZ: Flush To Zero ───────┐ ││││││││││││││
                               │┌┤│││││││││││││
               ┌──────────────┐││││││││││││││││
               │   reserved   │││││││││││││││││
             0b00000000000000000001111110000000 */
  uint32_t mxcsr;
  uint32_t mxcr_mask;

  struct FpuStackEntry st[8];
  struct XmmRegister xmm[16];
  uint32_t __padding[24];
};

typedef long long greg_t;
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
  uint64_t __pad[2];
  struct FpuState __fpustate; /* for cosmo on non-linux */
#elif defined(__aarch64__)
  sigset_t uc_sigmask;
  uint8_t __unused[1024 / 8];
  struct sigcontext uc_mcontext;
#endif
} forcealign(16);

typedef struct ucontext ucontext_t;

int getcontext(ucontext_t *) dontthrow __read_write(1);
int setcontext(const ucontext_t *) dontthrow __read_only(1);
int swapcontext(ucontext_t *, const ucontext_t *) dontthrow returnstwice;
void makecontext(ucontext_t *, void *, int, ...) dontthrow dontcallback;
void __sig_restore(const ucontext_t *) wontreturn;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_UCONTEXT_H_ */
