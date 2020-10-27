#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_FLAGS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_FLAGS_H_
#include "tool/build/lib/machine.h"

#define FLAGS_CF   0
#define FLAGS_VF   1
#define FLAGS_PF   2
#define FLAGS_F1   3
#define FLAGS_AF   4
#define FLAGS_KF   5
#define FLAGS_ZF   6
#define FLAGS_SF   7
#define FLAGS_TF   8
#define FLAGS_IF   9
#define FLAGS_DF   10
#define FLAGS_OF   11
#define FLAGS_IOPL 12
#define FLAGS_NT   14
#define FLAGS_F0   15
#define FLAGS_RF   16
#define FLAGS_VM   17
#define FLAGS_AC   18
#define FLAGS_VIF  19
#define FLAGS_VIP  20
#define FLAGS_ID   21

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define GetLazyParityBool(f)    GetParity((f) >> 24)
#define SetLazyParityByte(f, x) (((f) & ~0xFF000000u) | ((x)&0xFFu) << 24)

#define GetFlag(FLAGS, BIT)               \
  ({                                      \
    autotype(FLAGS) Flags = (FLAGS);      \
    typeof(Flags) IsSet;                  \
    switch (BIT) {                        \
      case FLAGS_PF:                      \
        IsSet = GetLazyParityBool(Flags); \
        break;                            \
      default:                            \
        IsSet = (Flags >> (BIT)) & 1;     \
        break;                            \
    }                                     \
    IsSet;                                \
  })

#define SetFlag(FLAGS, BIT, VAL)                          \
  ({                                                      \
    autotype(FLAGS) Flags = (FLAGS);                      \
    typeof(Flags) Val = (VAL);                            \
    typeof(Flags) One = 1;                                \
    switch (BIT) {                                        \
      case FLAGS_PF:                                      \
        Flags = SetLazyParityByte(Flags, !Val);           \
        break;                                            \
      default:                                            \
        Flags = (Flags & ~(One << (BIT))) | Val << (BIT); \
        break;                                            \
    }                                                     \
    Flags;                                                \
  })

bool GetParity(uint8_t);
uint64_t ExportFlags(uint64_t);
void ImportFlags(struct Machine *, uint64_t);
int64_t AluFlags(uint64_t, uint32_t, uint32_t *, uint32_t, uint32_t, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_FLAGS_H_ */
