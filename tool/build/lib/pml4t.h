#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define IsValidPage(x)    ((x)&1)
#define MaskPageAddr(x)   ((x)&0x00007ffffffff000)
#define UnmaskPageAddr(x) SignExtendAddr(MaskPageAddr(x))
#define SignExtendAddr(x) ((int64_t)((uint64_t)(x) << 16) >> 16)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_ */
