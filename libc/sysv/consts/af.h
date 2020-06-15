#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_AF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_AF_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long AF_ALG;
hidden extern const long AF_APPLETALK;
hidden extern const long AF_ASH;
hidden extern const long AF_ATMPVC;
hidden extern const long AF_ATMSVC;
hidden extern const long AF_AX25;
hidden extern const long AF_BLUETOOTH;
hidden extern const long AF_BRIDGE;
hidden extern const long AF_CAIF;
hidden extern const long AF_CAN;
hidden extern const long AF_ECONET;
hidden extern const long AF_FILE;
hidden extern const long AF_IB;
hidden extern const long AF_IEEE802154;
hidden extern const long AF_INET6;
hidden extern const long AF_INET;
hidden extern const long AF_IPX;
hidden extern const long AF_IRDA;
hidden extern const long AF_ISDN;
hidden extern const long AF_IUCV;
hidden extern const long AF_KCM;
hidden extern const long AF_KEY;
hidden extern const long AF_LLC;
hidden extern const long AF_LOCAL;
hidden extern const long AF_MAX;
hidden extern const long AF_MPLS;
hidden extern const long AF_NETBEUI;
hidden extern const long AF_NETLINK;
hidden extern const long AF_NETROM;
hidden extern const long AF_NFC;
hidden extern const long AF_PACKET;
hidden extern const long AF_PHONET;
hidden extern const long AF_PPPOX;
hidden extern const long AF_RDS;
hidden extern const long AF_ROSE;
hidden extern const long AF_ROUTE;
hidden extern const long AF_RXRPC;
hidden extern const long AF_SECURITY;
hidden extern const long AF_SNA;
hidden extern const long AF_TIPC;
hidden extern const long AF_UNIX;
hidden extern const long AF_UNSPEC;
hidden extern const long AF_VSOCK;
hidden extern const long AF_WANPIPE;
hidden extern const long AF_X25;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define AF_ALG SYMBOLIC(AF_ALG)
#define AF_APPLETALK SYMBOLIC(AF_APPLETALK)
#define AF_ASH SYMBOLIC(AF_ASH)
#define AF_ATMPVC SYMBOLIC(AF_ATMPVC)
#define AF_ATMSVC SYMBOLIC(AF_ATMSVC)
#define AF_AX25 SYMBOLIC(AF_AX25)
#define AF_BLUETOOTH SYMBOLIC(AF_BLUETOOTH)
#define AF_BRIDGE SYMBOLIC(AF_BRIDGE)
#define AF_CAIF SYMBOLIC(AF_CAIF)
#define AF_CAN SYMBOLIC(AF_CAN)
#define AF_ECONET SYMBOLIC(AF_ECONET)
#define AF_FILE SYMBOLIC(AF_FILE)
#define AF_IB SYMBOLIC(AF_IB)
#define AF_IEEE802154 SYMBOLIC(AF_IEEE802154)
#define AF_INET LITERALLY(2)
#define AF_INET6 SYMBOLIC(AF_INET6)
#define AF_IPX SYMBOLIC(AF_IPX)
#define AF_IRDA SYMBOLIC(AF_IRDA)
#define AF_ISDN SYMBOLIC(AF_ISDN)
#define AF_IUCV SYMBOLIC(AF_IUCV)
#define AF_KCM SYMBOLIC(AF_KCM)
#define AF_KEY SYMBOLIC(AF_KEY)
#define AF_LLC SYMBOLIC(AF_LLC)
#define AF_LOCAL SYMBOLIC(AF_LOCAL)
#define AF_MAX SYMBOLIC(AF_MAX)
#define AF_MPLS SYMBOLIC(AF_MPLS)
#define AF_NETBEUI SYMBOLIC(AF_NETBEUI)
#define AF_NETLINK SYMBOLIC(AF_NETLINK)
#define AF_NETROM SYMBOLIC(AF_NETROM)
#define AF_NFC SYMBOLIC(AF_NFC)
#define AF_PACKET SYMBOLIC(AF_PACKET)
#define AF_PHONET SYMBOLIC(AF_PHONET)
#define AF_PPPOX SYMBOLIC(AF_PPPOX)
#define AF_RDS SYMBOLIC(AF_RDS)
#define AF_ROSE SYMBOLIC(AF_ROSE)
#define AF_ROUTE SYMBOLIC(AF_ROUTE)
#define AF_RXRPC SYMBOLIC(AF_RXRPC)
#define AF_SECURITY SYMBOLIC(AF_SECURITY)
#define AF_SNA SYMBOLIC(AF_SNA)
#define AF_TIPC SYMBOLIC(AF_TIPC)
#define AF_UNIX LITERALLY(1)
#define AF_UNSPEC LITERALLY(0)
#define AF_VSOCK SYMBOLIC(AF_VSOCK)
#define AF_WANPIPE SYMBOLIC(AF_WANPIPE)
#define AF_X25 SYMBOLIC(AF_X25)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_AF_H_ */
