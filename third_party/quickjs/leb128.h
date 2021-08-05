#ifndef COSMOPOLITAN_THIRD_PARTY_QUICKJS_LEB128_H_
#define COSMOPOLITAN_THIRD_PARTY_QUICKJS_LEB128_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int get_leb128(uint32_t *, const uint8_t *, const uint8_t *);
int get_sleb128(int32_t *, const uint8_t *, const uint8_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_QUICKJS_LEB128_H_ */
