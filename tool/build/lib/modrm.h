#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_MODRM_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_MODRM_H_
#include "tool/build/lib/abp.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define IsModrmRegister(x)  (ModrmMod(x) == 3)
#define IsProbablyByteOp(x) !((x)->op.opcode & 1)
#define SibExists(x)        (ModrmRm(x) == 4)
#define SibHasIndex(x)      (SibIndex(x) != 4 || Rexx(x))
#define SibHasBase(x)       (SibBase(x) != 5 || ModrmMod(x))
#define SibIsAbsolute(x)    (!SibHasBase(x) && !SibHasIndex(x))
#define IsRipRelative(x)    (ModrmRm(x) == 5 && !ModrmMod(x))
#define SibBase(x)          (((x)->op.rde & 000007000000) >> 18)
#define SibIndex(x)         (((x)->op.rde & 000700000000) >> 24)
#define ModrmRm(x)          (((x)->op.rde & 000000000700) >> 6)
#define ModrmReg(x)         (((x)->op.rde & 000000000007) >> 0)
#define ModrmSrm(x)         (((x)->op.rde & 000000070000) >> 12)
#define ModrmMod(x)         (((x)->op.rde & 000060000000) >> 22)
#define RegLog2(x)          (((x)->op.rde & 006000000000) >> 28)
#define Rexx(x)             (((x)->op.rde & 001000000000) >> 27)
#define Asz(x)              (((x)->op.rde & 000000400000) >> 17)
#define Rexw(x)             (((x)->op.rde & 000000004000) >> 11)
#define Rexr(x)             (((x)->op.rde & 000000000010) >> 3)
#define Rexb(x)             (((x)->op.rde & 000010000000) >> 21)
#define Rex(x)              (((x)->op.rde & 000000000020) >> 4)
#define Osz(x)              (((x)->op.rde & 000000000040) >> 5)
#define Prefix66(x)         (((x)->op.rde & 010000000000) >> 30)
#define ByteRexrReg(m)      m->beg[(m->xedd->op.rde & 00000000037) >> 0]
#define ByteRexbRm(m)       m->beg[(m->xedd->op.rde & 00000003700) >> 6]
#define ByteRexbSrm(m)      m->beg[(m->xedd->op.rde & 00000370000) >> 12]
#define RegRexrReg(m)       Abp8(m->reg[(m->xedd->op.rde & 00000000017) >> 0])
#define RegRexbRm(m)        Abp8(m->reg[(m->xedd->op.rde & 00000001700) >> 6])
#define RegRexbSrm(m)       Abp8(m->reg[(m->xedd->op.rde & 00000170000) >> 12])
#define RegRexbBase(m)      Abp8(m->reg[(m->xedd->op.rde & 00017000000) >> 18])
#define RegRexxIndex(m)     Abp8(m->reg[(m->xedd->op.rde & 01700000000) >> 24])
#define XmmRexrReg(m)       Abp16(m->veg[(m->xedd->op.rde & 00000000017) >> 0])
#define XmmRexbRm(m)        Abp16(m->veg[(m->xedd->op.rde & 00000001700) >> 6])
#define MmReg(m)            Abp16(m->veg[(m->xedd->op.rde & 00000000007) >> 0])
#define MmRm(m)             Abp16(m->veg[(m->xedd->op.rde & 00000000700) >> 6])

int64_t ComputeAddress(const struct Machine *) nosideeffect;

void *ComputeReserveAddressRead(struct Machine *, size_t);
void *ComputeReserveAddressRead1(struct Machine *);
void *ComputeReserveAddressRead8(struct Machine *);
void *ComputeReserveAddressWrite(struct Machine *, size_t);
void *ComputeReserveAddressWrite1(struct Machine *);
void *ComputeReserveAddressWrite4(struct Machine *);
void *ComputeReserveAddressWrite8(struct Machine *);

uint8_t *GetModrmRegisterBytePointerRead(struct Machine *);
uint8_t *GetModrmRegisterBytePointerWrite(struct Machine *);
uint8_t *GetModrmRegisterMmPointerRead(struct Machine *, size_t);
uint8_t *GetModrmRegisterMmPointerRead8(struct Machine *);
uint8_t *GetModrmRegisterMmPointerWrite(struct Machine *, size_t);
uint8_t *GetModrmRegisterMmPointerWrite8(struct Machine *);
uint8_t *GetModrmRegisterWordPointerRead(struct Machine *, size_t);
uint8_t *GetModrmRegisterWordPointerRead2(struct Machine *);
uint8_t *GetModrmRegisterWordPointerRead4(struct Machine *);
uint8_t *GetModrmRegisterWordPointerRead8(struct Machine *);
uint8_t *GetModrmRegisterWordPointerReadOsz(struct Machine *);
uint8_t *GetModrmRegisterWordPointerReadOszRexw(struct Machine *);
uint8_t *GetModrmRegisterWordPointerWrite(struct Machine *, size_t);
uint8_t *GetModrmRegisterWordPointerWrite4(struct Machine *);
uint8_t *GetModrmRegisterWordPointerWrite8(struct Machine *);
uint8_t *GetModrmRegisterWordPointerWriteOsz(struct Machine *);
uint8_t *GetModrmRegisterWordPointerWriteOszRexw(struct Machine *);
uint8_t *GetModrmRegisterXmmPointerRead(struct Machine *, size_t);
uint8_t *GetModrmRegisterXmmPointerRead16(struct Machine *);
uint8_t *GetModrmRegisterXmmPointerRead4(struct Machine *);
uint8_t *GetModrmRegisterXmmPointerRead8(struct Machine *);
uint8_t *GetModrmRegisterXmmPointerWrite(struct Machine *, size_t);
uint8_t *GetModrmRegisterXmmPointerWrite16(struct Machine *);
uint8_t *GetModrmRegisterXmmPointerWrite4(struct Machine *);
uint8_t *GetModrmRegisterXmmPointerWrite8(struct Machine *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_MODRM_H_ */
