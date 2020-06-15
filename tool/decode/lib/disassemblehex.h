#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_DISASSEMBLEHEX_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_DISASSEMBLEHEX_H_

#define kDisassembleHexColumns 8

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void disassemblehex(uint8_t *data, size_t size, FILE *f);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_DISASSEMBLEHEX_H_ */
