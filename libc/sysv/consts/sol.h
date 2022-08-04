#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int SOL_AAL;
extern const int SOL_ALG;
extern const int SOL_ATM;
extern const int SOL_BLUETOOTH;
extern const int SOL_CAIF;
extern const int SOL_DCCP;
extern const int SOL_DECNET;
extern const int SOL_ICMPV6;
extern const int SOL_IP;
extern const int SOL_IPV6;
extern const int SOL_IRDA;
extern const int SOL_IUCV;
extern const int SOL_KCM;
extern const int SOL_LLC;
extern const int SOL_NETBEUI;
extern const int SOL_NETLINK;
extern const int SOL_NFC;
extern const int SOL_PACKET;
extern const int SOL_PNPIPE;
extern const int SOL_PPPOL2TP;
extern const int SOL_RAW;
extern const int SOL_RDS;
extern const int SOL_RXRPC;
extern const int SOL_SOCKET;
extern const int SOL_TCP;
extern const int SOL_TIPC;
extern const int SOL_UDP;
extern const int SOL_X25;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SOL_IP  LITERALLY(0)
#define SOL_TCP LITERALLY(6)
#define SOL_UDP LITERALLY(17)

#define SOL_AAL       SYMBOLIC(SOL_AAL)
#define SOL_ALG       SYMBOLIC(SOL_ALG)
#define SOL_ATM       SYMBOLIC(SOL_ATM)
#define SOL_BLUETOOTH SYMBOLIC(SOL_BLUETOOTH)
#define SOL_CAIF      SYMBOLIC(SOL_CAIF)
#define SOL_DCCP      SYMBOLIC(SOL_DCCP)
#define SOL_DECNET    SYMBOLIC(SOL_DECNET)
#define SOL_ICMPV6    SYMBOLIC(SOL_ICMPV6)
#define SOL_IPV6      SYMBOLIC(SOL_IPV6)
#define SOL_IRDA      SYMBOLIC(SOL_IRDA)
#define SOL_IUCV      SYMBOLIC(SOL_IUCV)
#define SOL_KCM       SYMBOLIC(SOL_KCM)
#define SOL_LLC       SYMBOLIC(SOL_LLC)
#define SOL_NETBEUI   SYMBOLIC(SOL_NETBEUI)
#define SOL_NETLINK   SYMBOLIC(SOL_NETLINK)
#define SOL_NFC       SYMBOLIC(SOL_NFC)
#define SOL_PACKET    SYMBOLIC(SOL_PACKET)
#define SOL_PNPIPE    SYMBOLIC(SOL_PNPIPE)
#define SOL_PPPOL2TP  SYMBOLIC(SOL_PPPOL2TP)
#define SOL_RAW       SYMBOLIC(SOL_RAW)
#define SOL_RDS       SYMBOLIC(SOL_RDS)
#define SOL_RXRPC     SYMBOLIC(SOL_RXRPC)
#define SOL_SOCKET    SYMBOLIC(SOL_SOCKET)
#define SOL_TIPC      SYMBOLIC(SOL_TIPC)
#define SOL_X25       SYMBOLIC(SOL_X25)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_ */
