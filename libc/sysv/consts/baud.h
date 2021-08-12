#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_BAUD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_BAUD_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long B0;
extern const long B50;
extern const long B75;
extern const long B110;
extern const long B134;
extern const long B150;
extern const long B200;
extern const long B300;
extern const long B600;
extern const long B1200;
extern const long B1800;
extern const long B2400;
extern const long B4800;
extern const long B9600;
extern const long B19200;
extern const long B38400;
extern const long B57600;
extern const long B115200;
extern const long B230400;
extern const long B500000;
extern const long B576000;
extern const long B1000000;
extern const long B1152000;
extern const long B1500000;
extern const long B2000000;
extern const long B2500000;
extern const long B3000000;
extern const long B3500000;
extern const long B4000000;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define B0       LITERALLY(0)
#define B50      SYMBOLIC(B50)
#define B75      SYMBOLIC(B75)
#define B110     SYMBOLIC(B110)
#define B134     SYMBOLIC(B134)
#define B150     SYMBOLIC(B150)
#define B200     SYMBOLIC(B200)
#define B300     SYMBOLIC(B300)
#define B600     SYMBOLIC(B600)
#define B1200    SYMBOLIC(B1200)
#define B1800    SYMBOLIC(B1800)
#define B2400    SYMBOLIC(B2400)
#define B4800    SYMBOLIC(B4800)
#define B9600    SYMBOLIC(B9600)
#define B19200   SYMBOLIC(B19200)
#define B38400   SYMBOLIC(B38400)
#define B57600   SYMBOLIC(B57600)
#define B115200  SYMBOLIC(B115200)
#define B230400  SYMBOLIC(B230400)
#define B500000  SYMBOLIC(B500000)
#define B576000  SYMBOLIC(B576000)
#define B1000000 SYMBOLIC(B1000000)
#define B1152000 SYMBOLIC(B1152000)
#define B1500000 SYMBOLIC(B1500000)
#define B2000000 SYMBOLIC(B2000000)
#define B2500000 SYMBOLIC(B2500000)
#define B3000000 SYMBOLIC(B3000000)
#define B3500000 SYMBOLIC(B3500000)
#define B4000000 SYMBOLIC(B4000000)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_BAUD_H_ */
