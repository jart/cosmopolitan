#ifndef COSMOPOLITAN_THIRD_PARTY_QUICKJS_LEB128_H_
#define COSMOPOLITAN_THIRD_PARTY_QUICKJS_LEB128_H_
COSMOPOLITAN_C_START_

int get_leb128(uint32_t *, const uint8_t *, const uint8_t *);
int get_sleb128(int32_t *, const uint8_t *, const uint8_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_QUICKJS_LEB128_H_ */
