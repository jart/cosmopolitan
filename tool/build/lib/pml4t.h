#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define IsValidPage(x)    ((x)&1)
#define UnmaskPageAddr(x) ((void *)SignExtendAddr(MaskPageAddr(x)))
#define MaskPageAddr(x)   ((int64_t)(intptr_t)(x)&0x00007ffffffff000)
#define SignExtendAddr(x) (!((x)&0x800000000000) ? (x) : (x) | -0x800000000000)

typedef uint64_t pml4t_t[512] aligned(4096);

int FreePml4t(pml4t_t, int64_t, uint64_t, void (*)(void *),
              int (*)(void *, size_t));
int RegisterPml4t(pml4t_t, int64_t, int64_t, size_t, void *(*)(void));
int64_t FindPml4t(pml4t_t, uint64_t, uint64_t);
char *FormatPml4t(pml4t_t) nodiscard;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_ */
