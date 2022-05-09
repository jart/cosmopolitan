#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long SOL_AAL;
extern const long SOL_ALG;
extern const long SOL_ATM;
extern const long SOL_BLUETOOTH;
extern const long SOL_CAIF;
extern const long SOL_DCCP;
extern const long SOL_DECNET;
extern const long SOL_ICMPV6;
extern const long SOL_IP;
extern const long SOL_IPV6;
extern const long SOL_IRDA;
extern const long SOL_IUCV;
extern const long SOL_KCM;
extern const long SOL_LLC;
extern const long SOL_NETBEUI;
extern const long SOL_NETLINK;
extern const long SOL_NFC;
extern const long SOL_PACKET;
extern const long SOL_PNPIPE;
extern const long SOL_PPPOL2TP;
extern const long SOL_RAW;
extern const long SOL_RDS;
extern const long SOL_RXRPC;
extern const long SOL_SOCKET;
extern const long SOL_TCP;
extern const long SOL_TIPC;
extern const long SOL_UDP;
extern const long SOL_X25;

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
