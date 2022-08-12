#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_LDBL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_LDBL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

double DeserializeLdbl(const uint8_t[10]);
uint8_t *SerializeLdbl(uint8_t[10], double);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_LDBL_H_ */
