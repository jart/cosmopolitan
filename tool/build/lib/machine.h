#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_MACHINE_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_MACHINE_H_
#include "libc/runtime/runtime.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/fds.h"

#define kMachineHalt                 -1
#define kMachineDecodeError          -2
#define kMachineUndefinedInstruction -3
#define kMachineSegmentationFault    -4
#define kMachineExit                 -5
#define kMachineDivideError          -6
#define kMachineFpuException         -7
#define kMachineProtectionFault      -8
#define kMachineSimdException        -9

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Machine {
  struct XedDecodedInst *xedd;
  uint64_t ip;
  uint8_t cs[8];
  uint8_t ss[8];
  uint64_t codevirt;
  uint8_t *codehost;
  uint32_t mode;
  uint32_t flags;
  uint32_t tlbindex;
  uint32_t stashsize;
  int64_t stashaddr;
  int64_t readaddr;
  int64_t writeaddr;
  uint32_t readsize;
  uint32_t writesize;
  union {
    uint8_t reg[16][8];
    struct {
      uint8_t ax[8];
      uint8_t cx[8];
      uint8_t dx[8];
      uint8_t bx[8];
      uint8_t sp[8];
      uint8_t bp[8];
      uint8_t si[8];
      uint8_t di[8];
      uint8_t r8[8];
      uint8_t r9[8];
      uint8_t r10[8];
      uint8_t r11[8];
      uint8_t r12[8];
      uint8_t r13[8];
      uint8_t r14[8];
      uint8_t r15[8];
    };
  };
  struct MachineTlb {
    int64_t virt;
    uint8_t *host;
  } tlb[16];
  struct MachineReal {
    size_t i, n;
    uint8_t *p;
  } real;
  uint64_t cr3;
  uint8_t xmm[16][16];
  uint8_t es[8];
  uint8_t ds[8];
  uint8_t fs[8];
  uint8_t gs[8];
  struct MachineFpu {
    long double st[8];
    union {
      uint32_t cw;
      struct {
        unsigned im : 1;   // invalid operation mask
        unsigned dm : 1;   // denormal operand mask
        unsigned zm : 1;   // zero divide mask
        unsigned om : 1;   // overflow mask
        unsigned um : 1;   // underflow mask
        unsigned pm : 1;   // precision mask
        unsigned _p1 : 2;  // reserved
        unsigned pc : 2;   // precision: 32,∅,64,80
        unsigned rc : 2;   // rounding: even,→-∞,→+∞,→0
      };
    };
    union {
      uint32_t sw;
      struct {
        unsigned ie : 1;  // invalid operation
        unsigned de : 1;  // denormalized operand
        unsigned ze : 1;  // zero divide
        unsigned oe : 1;  // overflow
        unsigned ue : 1;  // underflow
        unsigned pe : 1;  // precision
        unsigned sf : 1;  // stack fault
        unsigned es : 1;  // exception summary status
        unsigned c0 : 1;  // condition 0
        unsigned c1 : 1;  // condition 1
        unsigned c2 : 1;  // condition 2
        unsigned sp : 3;  // top stack
        unsigned c3 : 1;  // condition 3
        unsigned bf : 1;  // busy flag
      };
    };
    int tw;
    int op;
    int64_t ip;
    int64_t dp;
  } fpu;
  struct MachineSse {
    union {
      uint32_t mxcsr;
      struct {
        unsigned ie : 1;   // invalid operation flag
        unsigned de : 1;   // denormal flag
        unsigned ze : 1;   // divide by zero flag
        unsigned oe : 1;   // overflow flag
        unsigned ue : 1;   // underflow flag
        unsigned pe : 1;   // precision flag
        unsigned daz : 1;  // denormals are zeros
        unsigned im : 1;   // invalid operation mask
        unsigned dm : 1;   // denormal mask
        unsigned zm : 1;   // divide by zero mask
        unsigned om : 1;   // overflow mask
        unsigned um : 1;   // underflow mask
        unsigned pm : 1;   // precision mask
        unsigned rc : 2;   // rounding control
        unsigned ftz : 1;  // flush to zero
      };
    };
  } sse;
  uint64_t cr0;
  uint64_t cr2;
  uint64_t cr4;
  uint64_t gdt_base;
  uint64_t idt_base;
  uint16_t gdt_limit;
  uint16_t idt_limit;
  struct MachineRealFree {
    uint64_t i;
    uint64_t n;
    struct MachineRealFree *next;
  } * realfree;
  struct FreeList {
    uint32_t i;
    void *p[6];
  } freelist;
  struct MachineMemstat {
    int freed;
    int resizes;
    int reserved;
    int committed;
    int allocated;
    int reclaimed;
    int pagetables;
  } memstat;
  int64_t brk;
  int64_t bofram[2];
  jmp_buf onhalt;
  int64_t faultaddr;
  bool dlab;
  struct MachineFds fds;
  uint8_t stash[4096];
  uint8_t icache[1024][40];
  void (*onbinbase)(struct Machine *);
  void (*onlongbranch)(struct Machine *);
} forcealign(64);

struct Machine *NewMachine(void) nodiscard;
void FreeMachine(struct Machine *);
void ResetMem(struct Machine *);
void ResetCpu(struct Machine *);
void ResetTlb(struct Machine *);
void ResetInstructionCache(struct Machine *);
void LoadInstruction(struct Machine *);
void ExecuteInstruction(struct Machine *);
long AllocateLinearPage(struct Machine *);
long AllocateLinearPageRaw(struct Machine *);
int ReserveReal(struct Machine *, size_t);
int ReserveVirtual(struct Machine *, int64_t, size_t, uint64_t);
char *FormatPml4t(struct Machine *) nodiscard;
int64_t FindVirtual(struct Machine *, int64_t, size_t);
int FreeVirtual(struct Machine *, int64_t, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_MACHINE_H_ */
