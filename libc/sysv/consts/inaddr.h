#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_INADDR_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_INADDR_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Well-known Internet addresses.
 * These need to be hard-coded as little-endian, so htonl() is needed.
 */

hidden extern const long INADDR_ALLHOSTS_GROUP;
hidden extern const long INADDR_ALLRTRS_GROUP;
hidden extern const long INADDR_ANY;
hidden extern const long INADDR_BROADCAST;
hidden extern const long INADDR_LOOPBACK;
hidden extern const long INADDR_MAX_LOCAL_GROUP;
hidden extern const long INADDR_NONE;
hidden extern const long INADDR_TESTNET1;
hidden extern const long INADDR_TESTNET2;
hidden extern const long INADDR_TESTNET3;
hidden extern const long INADDR_UNSPEC_GROUP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define INADDR_ANY LITERALLY(0x00000000u)       /* 0.0.0.0 */
#define INADDR_BROADCAST LITERALLY(0xffffffffu) /* 255.255.255.255 */
#define INADDR_NONE LITERALLY(0xffffffffu)      /* 255.255.255.255 */
#define INADDR_LOOPBACK LITERALLY(0x7f000001u)  /* 127.0.0.1 */
#define INADDR_TESTNET1 LITERALLY(0xc0000200u)  /* 192.0.2.0/24 (RFC5737§3) */
#define INADDR_TESTNET2 LITERALLY(0xc6336400u)  /* 198.51.100.0/24 */
#define INADDR_TESTNET3 LITERALLY(0xcb007100u)  /* 203.0.113.0/24 */

#define INADDR_ALLHOSTS_GROUP SYMBOLIC(INADDR_ALLHOSTS_GROUP)
#define INADDR_ALLRTRS_GROUP SYMBOLIC(INADDR_ALLRTRS_GROUP)
#define INADDR_MAX_LOCAL_GROUP SYMBOLIC(INADDR_MAX_LOCAL_GROUP)
#define INADDR_UNSPEC_GROUP SYMBOLIC(INADDR_UNSPEC_GROUP)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_INADDR_H_ */
