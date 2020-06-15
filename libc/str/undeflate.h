#ifndef COSMOPOLITAN_LIBC_STR_UNDEFLATE_H_
#define COSMOPOLITAN_LIBC_STR_UNDEFLATE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct DeflateState {
  uint8_t lenlens[19];
  uint32_t lencodes[19];
  uint32_t distcodes[32];
  uint32_t litcodes[288];
  uint8_t lens[288 + 32];
};

ssize_t undeflate(void *output, size_t outputsize, void *input,
                  size_t inputsize, struct DeflateState *ds);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_UNDEFLATE_H_ */
