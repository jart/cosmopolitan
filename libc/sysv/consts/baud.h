#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_BAUD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_BAUD_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long __B0;
extern const long __B50;
extern const long __B75;
extern const long __B110;
extern const long __B134;
extern const long __B150;
extern const long __B200;
extern const long __B300;
extern const long __B600;
extern const long __B1200;
extern const long __B1800;
extern const long __B2400;
extern const long __B4800;
extern const long __B9600;
extern const long __B19200;
extern const long __B38400;
extern const long __B57600;
extern const long __B115200;
extern const long __B230400;
extern const long __B500000;
extern const long __B576000;
extern const long __B1000000;
extern const long __B1152000;
extern const long __B1500000;
extern const long __B2000000;
extern const long __B2500000;
extern const long __B3000000;
extern const long __B3500000;
extern const long __B4000000;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define __B0       LITERALLY(0)
#define __B50      SYMBOLIC(__B50)
#define __B75      SYMBOLIC(__B75)
#define __B110     SYMBOLIC(__B110)
#define __B134     SYMBOLIC(__B134)
#define __B150     SYMBOLIC(__B150)
#define __B200     SYMBOLIC(__B200)
#define __B300     SYMBOLIC(__B300)
#define __B600     SYMBOLIC(__B600)
#define __B1200    SYMBOLIC(__B1200)
#define __B1800    SYMBOLIC(__B1800)
#define __B2400    SYMBOLIC(__B2400)
#define __B4800    SYMBOLIC(__B4800)
#define __B9600    SYMBOLIC(__B9600)
#define __B19200   SYMBOLIC(__B19200)
#define __B38400   SYMBOLIC(__B38400)
#define __B57600   SYMBOLIC(__B57600)
#define __B115200  SYMBOLIC(__B115200)
#define __B230400  SYMBOLIC(__B230400)
#define __B500000  SYMBOLIC(__B500000)
#define __B576000  SYMBOLIC(__B576000)
#define __B1000000 SYMBOLIC(__B1000000)
#define __B1152000 SYMBOLIC(__B1152000)
#define __B1500000 SYMBOLIC(__B1500000)
#define __B2000000 SYMBOLIC(__B2000000)
#define __B2500000 SYMBOLIC(__B2500000)
#define __B3000000 SYMBOLIC(__B3000000)
#define __B3500000 SYMBOLIC(__B3500000)
#define __B4000000 SYMBOLIC(__B4000000)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_BAUD_H_ */
