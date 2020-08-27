#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_MODRM_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_MODRM_H_
#include "tool/build/lib/abp.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define SibBase(x)  ((x & 000007000000) >> 022)
#define SibIndex(x) ((x & 000700000000) >> 030)
#define ModrmRm(x)  ((x & 000000000700) >> 006)
#define ModrmReg(x) ((x & 000000000007) >> 000)
#define ModrmSrm(x) ((x & 000000070000) >> 014)
#define ModrmMod(x) ((x & 000060000000) >> 026)
#define RegLog2(x)  ((x & 006000000000) >> 034)
#define Rexx(x)     ((x & 001000000000) >> 033)
#define Asz(x)      ((x & 000000400000) >> 021)
#define Rexw(x)     ((x & 000000004000) >> 013)
#define Rexr(x)     ((x & 000000000010) >> 003)
#define Rexb(x)     ((x & 000010000000) >> 025)
#define Rex(x)      ((x & 000000000020) >> 004)
#define Osz(x)      ((x & 000000000040) >> 005)
#define Rep(x)      ((x & 030000000000) >> 036)

#define IsModrmRegister(x) (ModrmMod(x) == 3)
#define SibExists(x)       (ModrmRm(x) == 4)
#define SibHasIndex(x)     (SibIndex(x) != 4 || Rexx(x))
#define SibHasBase(x)      (SibBase(x) != 5 || ModrmMod(x))
#define SibIsAbsolute(x)   (!SibHasBase(x) && !SibHasIndex(x))
#define IsRipRelative(x)   (ModrmRm(x) == 5 && !ModrmMod(x))

#define ByteRexrReg(m, x)  m->beg[(x & 00000000037) >> 0]
#define ByteRexbRm(m, x)   m->beg[(x & 00000003700) >> 6]
#define ByteRexbSrm(m, x)  m->beg[(x & 00000370000) >> 12]
#define RegRexbSrm(m, x)   Abp8(m->reg[(x & 00000170000) >> 12])
#define RegRexrReg(m, x)   Abp8(m->reg[(x & 00000000017) >> 0])
#define RegRexbRm(m, x)    Abp8(m->reg[(x & 00000001700) >> 6])
#define RegRexbBase(m, x)  Abp8(m->reg[(x & 00017000000) >> 18])
#define RegRexxIndex(m, x) Abp8(m->reg[(x & 01700000000) >> 24])
#define XmmRexrReg(m, x)   Abp16(m->veg[(x & 00000000017) >> 0])
#define XmmRexbRm(m, x)    Abp16(m->veg[(x & 00000001700) >> 6])
#define MmReg(m, x)        Abp16(m->veg[(x & 00000000007) >> 0])
#define MmRm(m, x)         Abp16(m->veg[(x & 00000000700) >> 6])

int64_t ComputeAddress(const struct Machine *, uint32_t) nosideeffect;

void *ComputeReserveAddressRead(struct Machine *, uint32_t, size_t);
void *ComputeReserveAddressRead1(struct Machine *, uint32_t);
void *ComputeReserveAddressRead8(struct Machine *, uint32_t);
void *ComputeReserveAddressWrite(struct Machine *, uint32_t, size_t);
void *ComputeReserveAddressWrite1(struct Machine *, uint32_t);
void *ComputeReserveAddressWrite4(struct Machine *, uint32_t);
void *ComputeReserveAddressWrite8(struct Machine *, uint32_t);

uint8_t *GetModrmRegisterBytePointerRead(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterBytePointerWrite(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterMmPointerRead(struct Machine *, uint32_t, size_t);
uint8_t *GetModrmRegisterMmPointerRead8(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterMmPointerWrite(struct Machine *, uint32_t, size_t);
uint8_t *GetModrmRegisterMmPointerWrite8(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerRead(struct Machine *, uint32_t, size_t);
uint8_t *GetModrmRegisterWordPointerRead2(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerRead4(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerRead8(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerReadOsz(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerReadOszRexw(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerWrite(struct Machine *, uint32_t, size_t);
uint8_t *GetModrmRegisterWordPointerWrite4(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerWrite8(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerWriteOsz(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterWordPointerWriteOszRexw(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterXmmPointerRead(struct Machine *, uint32_t, size_t);
uint8_t *GetModrmRegisterXmmPointerRead16(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterXmmPointerRead4(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterXmmPointerRead8(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterXmmPointerWrite(struct Machine *, uint32_t, size_t);
uint8_t *GetModrmRegisterXmmPointerWrite16(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterXmmPointerWrite4(struct Machine *, uint32_t);
uint8_t *GetModrmRegisterXmmPointerWrite8(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_MODRM_H_ */
