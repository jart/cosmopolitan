#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ALU_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ALU_H_
#include "tool/build/lib/machine.h"

#define ALU_ADD 0
#define ALU_OR  1
#define ALU_ADC 2
#define ALU_SBB 3
#define ALU_AND 4
#define ALU_SUB 5
#define ALU_XOR 6
#define ALU_CMP 7
#define ALU_NOT 8
#define ALU_NEG 9
#define ALU_INC 10
#define ALU_DEC 11

#define BSU_ROL 0
#define BSU_ROR 1
#define BSU_RCL 2
#define BSU_RCR 3
#define BSU_SHL 4
#define BSU_SHR 5
#define BSU_SAL 6
#define BSU_SAR 7

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef int64_t (*aluop_f)(uint64_t, uint64_t, uint32_t *);

extern const aluop_f kAlu[12][4];
extern const aluop_f kBsu[8][4];

int64_t Xor8(uint64_t, uint64_t, uint32_t *);
int64_t Xor16(uint64_t, uint64_t, uint32_t *);
int64_t Xor32(uint64_t, uint64_t, uint32_t *);
int64_t Xor64(uint64_t, uint64_t, uint32_t *);
int64_t Or8(uint64_t, uint64_t, uint32_t *);
int64_t Or16(uint64_t, uint64_t, uint32_t *);
int64_t Or32(uint64_t, uint64_t, uint32_t *);
int64_t Or64(uint64_t, uint64_t, uint32_t *);
int64_t And8(uint64_t, uint64_t, uint32_t *);
int64_t And16(uint64_t, uint64_t, uint32_t *);
int64_t And32(uint64_t, uint64_t, uint32_t *);
int64_t And64(uint64_t, uint64_t, uint32_t *);
int64_t Sub8(uint64_t, uint64_t, uint32_t *);
int64_t Sbb8(uint64_t, uint64_t, uint32_t *);
int64_t Sub16(uint64_t, uint64_t, uint32_t *);
int64_t Sbb16(uint64_t, uint64_t, uint32_t *);
int64_t Sub32(uint64_t, uint64_t, uint32_t *);
int64_t Sbb32(uint64_t, uint64_t, uint32_t *);
int64_t Sub64(uint64_t, uint64_t, uint32_t *);
int64_t Sbb64(uint64_t, uint64_t, uint32_t *);
int64_t Add8(uint64_t, uint64_t, uint32_t *);
int64_t Adc8(uint64_t, uint64_t, uint32_t *);
int64_t Add16(uint64_t, uint64_t, uint32_t *);
int64_t Adc16(uint64_t, uint64_t, uint32_t *);
int64_t Add32(uint64_t, uint64_t, uint32_t *);
int64_t Adc32(uint64_t, uint64_t, uint32_t *);
int64_t Add64(uint64_t, uint64_t, uint32_t *);
int64_t Adc64(uint64_t, uint64_t, uint32_t *);
int64_t Not8(uint64_t, uint64_t, uint32_t *);
int64_t Not16(uint64_t, uint64_t, uint32_t *);
int64_t Not32(uint64_t, uint64_t, uint32_t *);
int64_t Not64(uint64_t, uint64_t, uint32_t *);
int64_t Neg8(uint64_t, uint64_t, uint32_t *);
int64_t Neg16(uint64_t, uint64_t, uint32_t *);
int64_t Neg32(uint64_t, uint64_t, uint32_t *);
int64_t Neg64(uint64_t, uint64_t, uint32_t *);
int64_t Inc8(uint64_t, uint64_t, uint32_t *);
int64_t Inc16(uint64_t, uint64_t, uint32_t *);
int64_t Inc32(uint64_t, uint64_t, uint32_t *);
int64_t Inc64(uint64_t, uint64_t, uint32_t *);
int64_t Dec8(uint64_t, uint64_t, uint32_t *);
int64_t Dec16(uint64_t, uint64_t, uint32_t *);
int64_t Dec32(uint64_t, uint64_t, uint32_t *);
int64_t Dec64(uint64_t, uint64_t, uint32_t *);

int64_t Shr8(uint64_t, uint64_t, uint32_t *);
int64_t Shr16(uint64_t, uint64_t, uint32_t *);
int64_t Shr32(uint64_t, uint64_t, uint32_t *);
int64_t Shr64(uint64_t, uint64_t, uint32_t *);
int64_t Shl8(uint64_t, uint64_t, uint32_t *);
int64_t Shl16(uint64_t, uint64_t, uint32_t *);
int64_t Shl32(uint64_t, uint64_t, uint32_t *);
int64_t Shl64(uint64_t, uint64_t, uint32_t *);
int64_t Sar8(uint64_t, uint64_t, uint32_t *);
int64_t Sar16(uint64_t, uint64_t, uint32_t *);
int64_t Sar32(uint64_t, uint64_t, uint32_t *);
int64_t Sar64(uint64_t, uint64_t, uint32_t *);
int64_t Rol8(uint64_t, uint64_t, uint32_t *);
int64_t Rol16(uint64_t, uint64_t, uint32_t *);
int64_t Rol32(uint64_t, uint64_t, uint32_t *);
int64_t Rol64(uint64_t, uint64_t, uint32_t *);
int64_t Ror8(uint64_t, uint64_t, uint32_t *);
int64_t Ror16(uint64_t, uint64_t, uint32_t *);
int64_t Ror32(uint64_t, uint64_t, uint32_t *);
int64_t Ror64(uint64_t, uint64_t, uint32_t *);
int64_t Rcr8(uint64_t, uint64_t, uint32_t *);
int64_t Rcr16(uint64_t, uint64_t, uint32_t *);
int64_t Rcr32(uint64_t, uint64_t, uint32_t *);
int64_t Rcr64(uint64_t, uint64_t, uint32_t *);
int64_t Rcl8(uint64_t, uint64_t, uint32_t *);
int64_t Rcl16(uint64_t, uint64_t, uint32_t *);
int64_t Rcl32(uint64_t, uint64_t, uint32_t *);
int64_t Rcl64(uint64_t, uint64_t, uint32_t *);

uint64_t BsuDoubleShift(int, uint64_t, uint64_t, uint8_t, bool, uint32_t *);

int64_t AluFlags8(uint8_t, uint32_t, uint32_t *, uint32_t, uint32_t);
int64_t AluFlags16(uint16_t, uint32_t, uint32_t *, uint32_t, uint32_t);
int64_t AluFlags32(uint32_t, uint32_t, uint32_t *, uint32_t, uint32_t);
int64_t AluFlags64(uint64_t, uint32_t, uint32_t *, uint32_t, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ALU_H_ */
