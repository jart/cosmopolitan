#ifndef COSMOPOLITAN_LIBC_CALLS_TERMIOS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_TERMIOS_INTERNAL_H_
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/struct/termios.h"

#define COPY_TERMIOS(TO, FROM)                               \
  do {                                                       \
    uint32_t Cc3;                                            \
    uint64_t Cc1, Cc2;                                       \
    autotype((TO)->c_iflag) c_iflag = (FROM)->c_iflag;       \
    autotype((TO)->c_oflag) c_oflag = (FROM)->c_oflag;       \
    autotype((TO)->c_cflag) c_cflag = (FROM)->c_cflag;       \
    autotype((TO)->c_lflag) c_lflag = (FROM)->c_lflag;       \
    __builtin_memcpy(&Cc1, (FROM)->c_cc + 000, 8);           \
    __builtin_memcpy(&Cc2, (FROM)->c_cc + 010, 8);           \
    __builtin_memcpy(&Cc3, (FROM)->c_cc + 020, 4);           \
    autotype((TO)->_c_ispeed) _c_ispeed = (FROM)->_c_ispeed; \
    autotype((TO)->_c_ospeed) _c_ospeed = (FROM)->_c_ospeed; \
    (TO)->c_iflag = c_iflag;                                 \
    (TO)->c_oflag = c_oflag;                                 \
    (TO)->c_cflag = c_cflag;                                 \
    (TO)->c_lflag = c_lflag;                                 \
    __builtin_memcpy((TO)->c_cc + 000, &Cc1, 8);             \
    __builtin_memcpy((TO)->c_cc + 010, &Cc2, 8);             \
    __builtin_memcpy((TO)->c_cc + 020, &Cc3, 4);             \
    (TO)->_c_ispeed = _c_ispeed;                             \
    /* null c_ospeed causes bsd to hangup terminal */        \
    (TO)->_c_ospeed = _c_ospeed ? _c_ospeed : 9600;          \
  } while (0)

void *__termios2host(union metatermios *, const struct termios *);

#endif /* COSMOPOLITAN_LIBC_CALLS_TERMIOS_INTERNAL_H_ */
