#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct termios {     /* GNU/Systemd ABI */
  uint32_t c_iflag;  /* input modes */
  uint32_t c_oflag;  /* output modes */
  uint32_t c_cflag;  /* control modes */
  uint32_t c_lflag;  /* local modes */
  uint8_t c_cc[32];  /* code mappings */
  uint32_t c_ispeed; /* input speed */
  uint32_t c_ospeed; /* output speed */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_H_ */
