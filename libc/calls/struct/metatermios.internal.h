#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_METATERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_METATERMIOS_H_
#include "libc/calls/struct/termios.h"
COSMOPOLITAN_C_START_

struct termios_xnu {
  uint64_t c_iflag;
  uint64_t c_oflag;
  uint64_t c_cflag;
  uint64_t c_lflag;
  uint8_t c_cc[20];
  uint64_t _c_ispeed;
  uint64_t _c_ospeed;
};

struct termios_bsd {
  uint32_t c_iflag;
  uint32_t c_oflag;
  uint32_t c_cflag;
  uint32_t c_lflag;
  uint8_t c_cc[20];
  uint32_t _c_ispeed;
  uint32_t _c_ospeed;
};

union metatermios {
  struct termios linux;
  struct termios_xnu xnu;
  struct termios_bsd bsd;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_METATERMIOS_H_ */
