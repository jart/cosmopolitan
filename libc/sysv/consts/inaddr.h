#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_INADDR_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_INADDR_H_

/**
 * @fileoverview Well-known Internet addresses.
 * These need to be hard-coded as little-endian, so htonl() is needed.
 */

#define INADDR_ANY       0x00000000u /* 0.0.0.0 */
#define INADDR_BROADCAST 0xFFFFFFFFu /* 255.255.255.255 */
#define INADDR_NONE      0xFFFFFFFFu /* 255.255.255.255 */
#define INADDR_LOOPBACK  0x7F000001u /* 127.0.0.1 */
#define INADDR_TESTNET1  0xC0000200u /* 192.0.2.0/24 (RFC5737§3) */
#define INADDR_TESTNET2  0xC6336400u /* 198.51.100.0/24 */
#define INADDR_TESTNET3  0xCB007100u /* 203.0.113.0/24 */

#define INADDR_ALLHOSTS_GROUP  0xE0000001u
#define INADDR_ALLRTRS_GROUP   0xE0000002u
#define INADDR_MAX_LOCAL_GROUP 0xE00000FFu
#define INADDR_UNSPEC_GROUP    0xE0000000u

#define IN_LOOPBACKNET 127

/* clang-format off */
#define IN6ADDR_ANY_INIT      { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
#define IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }
/* clang-format on */

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_INADDR_H_ */
