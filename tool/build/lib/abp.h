#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ABP_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ABP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define Abp8(x)  ((uint8_t *)__builtin_assume_aligned(x, 8))
#define Abp16(x) ((uint8_t *)__builtin_assume_aligned(x, 16))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ABP_H_ */
