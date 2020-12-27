#ifndef COSMOPOLITAN_LIBC_CALLS_TERMIOS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_TERMIOS_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/bits/safemacros.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define COPY_TERMIOS(TO, FROM)                             \
  do {                                                     \
    memset((TO), 0, sizeof(*(TO)));                        \
    (TO)->c_iflag = (FROM)->c_iflag;                       \
    (TO)->c_oflag = (FROM)->c_oflag;                       \
    (TO)->c_cflag = (FROM)->c_cflag;                       \
    (TO)->c_lflag = (FROM)->c_lflag;                       \
    memcpy((TO)->c_cc, (FROM)->c_cc,                       \
           min(sizeof((TO)->c_cc), sizeof((FROM)->c_cc))); \
    (TO)->c_ispeed = (FROM)->c_ispeed;                     \
    (TO)->c_ospeed = (FROM)->c_ospeed;                     \
  } while (0)

void *termios2host(union metatermios *, const struct termios *);
void termios2linux(struct termios *, const union metatermios *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_CALLS_TERMIOS_INTERNAL_H_ */
