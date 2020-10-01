#ifndef COSMOPOLITAN_TOOL_BUILD_EMUBIN_REAL_H_
#define COSMOPOLITAN_TOOL_BUILD_EMUBIN_REAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline void SetEs(int base) {
  asm volatile("mov%z0\t%0,%%es" : /* no outputs */ : "r"(base));
}

forceinline void SetVideoMode(int mode) {
  asm volatile("int\t$0x10"
               : /* no outputs */
               : "a"(mode));
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_EMUBIN_REAL_H_ */
