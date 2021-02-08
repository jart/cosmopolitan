#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SOCK_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long SOCK_CLOEXEC;
extern const long SOCK_DCCP;
extern const long SOCK_DGRAM;
extern const long SOCK_NONBLOCK;
extern const long SOCK_PACKET;
extern const long SOCK_RAW;
extern const long SOCK_RDM;
extern const long SOCK_SEQPACKET;
extern const long SOCK_STREAM;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SOCK_CLOEXEC SYMBOLIC(SOCK_CLOEXEC)
#define SOCK_DCCP SYMBOLIC(SOCK_DCCP)
#define SOCK_DGRAM LITERALLY(2)
#define SOCK_NONBLOCK SYMBOLIC(SOCK_NONBLOCK)
#define SOCK_PACKET SYMBOLIC(SOCK_PACKET)
#define SOCK_RAW LITERALLY(3)
#define SOCK_RDM LITERALLY(4)
#define SOCK_SEQPACKET LITERALLY(5)
#define SOCK_STREAM LITERALLY(1)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SOCK_H_ */
