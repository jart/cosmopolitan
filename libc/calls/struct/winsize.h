#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct winsize {
  uint16_t ws_row;
  uint16_t ws_col;
  uint16_t ws_xpixel;
  uint16_t ws_ypixel;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_H_ */
