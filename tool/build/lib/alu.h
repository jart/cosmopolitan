#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ALU_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ALU_H_
#include "tool/build/lib/machine.h"

#define ALU_ADD  0
#define ALU_OR   1
#define ALU_ADC  2
#define ALU_SBB  3
#define ALU_AND  4
#define ALU_SUB  5
#define ALU_XOR  6
#define ALU_CMP  7
#define ALU_TEST 8
#define ALU_FLIP 16

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

typedef uint64_t (*aluop1_f)(struct Machine *, uint32_t, uint64_t);
typedef uint64_t (*aluop2_f)(struct Machine *, uint32_t, uint64_t, uint64_t);

int64_t Alu(int, int, uint64_t, uint64_t, uint32_t *);
int64_t Bsu(int, int, uint64_t, uint64_t, uint32_t *);
uint64_t AluBt(struct Machine *, uint64_t, uint64_t);
uint64_t AluBtc(struct Machine *, uint64_t, uint64_t);
uint64_t AluBtr(struct Machine *, uint64_t, uint64_t);
uint64_t AluBts(struct Machine *, uint64_t, uint64_t);
uint64_t BsuDoubleShift(int, uint64_t, uint64_t, uint8_t, bool, uint32_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ALU_H_ */
