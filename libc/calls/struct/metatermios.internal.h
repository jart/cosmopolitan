#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_METATERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_METATERMIOS_H_
#ifndef __STRICT_ANSI__
#include "libc/calls/struct/termios.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct termios$xnu {
  uint64_t c_iflag;
  uint64_t c_oflag;
  uint64_t c_cflag;
  uint64_t c_lflag;
  uint8_t c_cc[20];
  uint64_t c_ispeed;
  uint64_t c_ospeed;
};

struct termios$bsd {
  uint32_t c_iflag;
  uint32_t c_oflag;
  uint32_t c_cflag;
  uint32_t c_lflag;
  uint8_t c_cc[20];
  uint32_t c_ispeed;
  uint32_t c_ospeed;
};

union metatermios {
  struct termios linux;
  struct termios$xnu xnu;
  struct termios$bsd bsd;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_METATERMIOS_H_ */
