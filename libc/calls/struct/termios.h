#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_H_

#define NCCS 20

COSMOPOLITAN_C_START_

struct termios {      /* cosmo abi */
  uint32_t c_iflag;   /* input modes */
  uint32_t c_oflag;   /* output modes */
  uint32_t c_cflag;   /* control modes */
  uint32_t c_lflag;   /* local modes */
  uint8_t c_cc[NCCS]; /* code mappings */
  uint32_t _c_ispeed; /* use cfgetispeed() and cfsetispeed() */
  uint32_t _c_ospeed; /* use cfgetospeed() and cfsetospeed() */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_H_ */
