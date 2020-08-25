#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_COND_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_COND_H_
#include "tool/build/lib/flags.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline bool GetCond(struct Machine *m, int x) {
  uint32_t f = m->flags;
  switch (x) {
    case 0:
      return GetFlag(f, FLAGS_OF);
    case 1:
      return !GetFlag(f, FLAGS_OF);
    case 2:
      return GetFlag(f, FLAGS_CF);
    case 3:
      return !GetFlag(f, FLAGS_CF);
    case 4:
      return GetFlag(f, FLAGS_ZF);
    case 5:
      return !GetFlag(f, FLAGS_ZF);
    case 6:
      return GetFlag(f, FLAGS_CF) || GetFlag(f, FLAGS_ZF);
    case 7:
      return !GetFlag(f, FLAGS_CF) && !GetFlag(f, FLAGS_ZF);
    case 8:
      return GetFlag(f, FLAGS_SF);
    case 9:
      return !GetFlag(f, FLAGS_SF);
    case 10:
      return GetFlag(f, FLAGS_PF);
    case 11:
      return !GetFlag(f, FLAGS_PF);
    case 12:
      return GetFlag(f, FLAGS_SF) != GetFlag(f, FLAGS_OF);
    case 13:
      return GetFlag(f, FLAGS_SF) == GetFlag(f, FLAGS_OF);
    case 14:
      return GetFlag(f, FLAGS_ZF) ||
             GetFlag(f, FLAGS_SF) != GetFlag(f, FLAGS_OF);
    case 15:
      return !GetFlag(f, FLAGS_ZF) &&
             GetFlag(f, FLAGS_SF) == GetFlag(f, FLAGS_OF);
    default:
      unreachable;
  }
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_COND_H_ */
