#ifndef COSMOPOLITAN_TOOL_BUILD_EMUBIN_POKE_H_
#define COSMOPOLITAN_TOOL_BUILD_EMUBIN_POKE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define POKE(MEM, VAL) \
  asm volatile("mov%z0\t%1,%%es:%0" : "=m"(MEM) : "Qi"((typeof(MEM))(VAL)))

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_EMUBIN_POKE_H_ */
