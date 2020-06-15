#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SOCK_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long SOCK_CLOEXEC;
hidden extern const long SOCK_DCCP;
hidden extern const long SOCK_DGRAM;
hidden extern const long SOCK_NONBLOCK;
hidden extern const long SOCK_PACKET;
hidden extern const long SOCK_RAW;
hidden extern const long SOCK_RDM;
hidden extern const long SOCK_SEQPACKET;
hidden extern const long SOCK_STREAM;

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
