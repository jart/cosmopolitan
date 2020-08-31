#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_MACHINE_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_MACHINE_H_
#include "libc/elf/struct/ehdr.h"
#include "libc/runtime/runtime.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/fds.h"
#include "tool/build/lib/pml4t.h"

#define kXmmIntegral 0
#define kXmmDouble   1
#define kXmmFloat    2

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
  uint64_t codevirt;
  uint8_t *codereal;
  uint32_t flags;
  uint32_t stashsize;
  int64_t stashaddr;
  int64_t readaddr;
  int64_t writeaddr;
  uint32_t readsize;
  uint32_t writesize;
  union {
    uint8_t reg[2 * 8][8];
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
  } aligned(8);
  uint32_t tlbindex;
  struct TlbEntry {
    int64_t v;
    void *r;
  } tlb[16];
  uint8_t *veg[2 * 8];
  uint8_t *beg[2 * 2 * 8];
  struct MachineFpu {
    long double st[8];
    union {
      uint32_t cw;
      struct {
        unsigned im : 1;  /* invalid operation mask */
        unsigned dm : 1;  /* denormal operand mask */
        unsigned zm : 1;  /* zero divide mask */
        unsigned om : 1;  /* overflow mask */
        unsigned um : 1;  /* underflow mask */
        unsigned pm : 1;  /* precision mask */
        unsigned _p1 : 2; /* reserved */
        unsigned pc : 2;  /* precision: 32,∅,64,80 */
        unsigned rc : 2;  /* rounding: even,→-∞,→+∞,→0 */
      };
    };
    union {
      uint32_t sw;
      struct {
        unsigned ie : 1; /* invalid operation */
        unsigned de : 1; /* denormalized operand */
        unsigned ze : 1; /* zero divide */
        unsigned oe : 1; /* overflow */
        unsigned ue : 1; /* underflow */
        unsigned pe : 1; /* precision */
        unsigned sf : 1; /* stack fault */
        unsigned es : 1; /* exception summary status */
        unsigned c0 : 1; /* condition 0 */
        unsigned c1 : 1; /* condition 1 */
        unsigned c2 : 1; /* condition 2 */
        unsigned sp : 3; /* top stack */
        unsigned c3 : 1; /* condition 3 */
        unsigned bf : 1; /* busy flag */
      };
    };
    int tw;
    int op;
    int64_t ip;
    int64_t dp;
  } fpu;
  struct {
    union {
      uint32_t mxcsr;
      struct {
        unsigned ie : 1;  /* invalid operation flag */
        unsigned de : 1;  /* denormal flag */
        unsigned ze : 1;  /* divide by zero flag */
        unsigned oe : 1;  /* overflow flag */
        unsigned ue : 1;  /* underflow flag */
        unsigned pe : 1;  /* precision flag */
        unsigned daz : 1; /* denormals are zeros */
        unsigned im : 1;  /* invalid operation mask */
        unsigned dm : 1;  /* denormal mask */
        unsigned zm : 1;  /* divide by zero mask */
        unsigned om : 1;  /* overflow mask */
        unsigned um : 1;  /* underflow mask */
        unsigned pm : 1;  /* precision mask */
        unsigned rc : 2;  /* rounding control */
        unsigned ftz : 1; /* flush to zero */
      };
    };
  } sse;
  struct FreeList {
    uint32_t i;
    void *p[6];
  } freelist;
  pml4t_t cr3;
  uint8_t xmm[2][8][16] aligned(16);
  int64_t bofram[2];
  jmp_buf onhalt;
  int64_t faultaddr;
  uint8_t stash[4096];
  uint8_t xmmtype[2][8];
  struct XedDecodedInst icache[512];
  struct MachineFds fds;
};

void ResetCpu(struct Machine *);
void LoadInstruction(struct Machine *);
void ExecuteInstruction(struct Machine *);
struct Machine *NewMachine(void) nodiscard;
void LoadArgv(struct Machine *, const char *, char **, char **);
void InitMachine(struct Machine *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_MACHINE_H_ */
