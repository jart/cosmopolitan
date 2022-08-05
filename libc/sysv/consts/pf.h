#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PF_H_
#include "libc/runtime/symbolic.h"

#define PF_ALG SYMBOLIC(PF_ALG)
#define PF_APPLETALK SYMBOLIC(PF_APPLETALK)
#define PF_ASH SYMBOLIC(PF_ASH)
#define PF_ATMPVC SYMBOLIC(PF_ATMPVC)
#define PF_ATMSVC SYMBOLIC(PF_ATMSVC)
#define PF_AX25 SYMBOLIC(PF_AX25)
#define PF_BLUETOOTH SYMBOLIC(PF_BLUETOOTH)
#define PF_BRIDGE SYMBOLIC(PF_BRIDGE)
#define PF_CAIF SYMBOLIC(PF_CAIF)
#define PF_CAN SYMBOLIC(PF_CAN)
#define PF_ECONET SYMBOLIC(PF_ECONET)
#define PF_FILE SYMBOLIC(PF_FILE)
#define PF_IB SYMBOLIC(PF_IB)
#define PF_IEEE802154 SYMBOLIC(PF_IEEE802154)
#define PF_INET SYMBOLIC(PF_INET)
#define PF_INET6 SYMBOLIC(PF_INET6)
#define PF_IPX SYMBOLIC(PF_IPX)
#define PF_IRDA SYMBOLIC(PF_IRDA)
#define PF_ISDN SYMBOLIC(PF_ISDN)
#define PF_IUCV SYMBOLIC(PF_IUCV)
#define PF_KCM SYMBOLIC(PF_KCM)
#define PF_KEY SYMBOLIC(PF_KEY)
#define PF_LLC SYMBOLIC(PF_LLC)
#define PF_LOCAL SYMBOLIC(PF_LOCAL)
#define PF_MAX SYMBOLIC(PF_MAX)
#define PF_MPLS SYMBOLIC(PF_MPLS)
#define PF_NETBEUI SYMBOLIC(PF_NETBEUI)
#define PF_NETLINK SYMBOLIC(PF_NETLINK)
#define PF_NETROM SYMBOLIC(PF_NETROM)
#define PF_NFC SYMBOLIC(PF_NFC)
#define PF_PACKET SYMBOLIC(PF_PACKET)
#define PF_PHONET SYMBOLIC(PF_PHONET)
#define PF_PPPOX SYMBOLIC(PF_PPPOX)
#define PF_RDS SYMBOLIC(PF_RDS)
#define PF_ROSE SYMBOLIC(PF_ROSE)
#define PF_ROUTE SYMBOLIC(PF_ROUTE)
#define PF_RXRPC SYMBOLIC(PF_RXRPC)
#define PF_SECURITY SYMBOLIC(PF_SECURITY)
#define PF_SNA SYMBOLIC(PF_SNA)
#define PF_TIPC SYMBOLIC(PF_TIPC)
#define PF_UNIX SYMBOLIC(PF_UNIX)
#define PF_UNSPEC SYMBOLIC(PF_UNSPEC)
#define PF_VSOCK SYMBOLIC(PF_VSOCK)
#define PF_WANPIPE SYMBOLIC(PF_WANPIPE)
#define PF_X25 SYMBOLIC(PF_X25)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int PF_ALG;
extern const int PF_APPLETALK;
extern const int PF_ASH;
extern const int PF_ATMPVC;
extern const int PF_ATMSVC;
extern const int PF_AX25;
extern const int PF_BLUETOOTH;
extern const int PF_BRIDGE;
extern const int PF_CAIF;
extern const int PF_CAN;
extern const int PF_ECONET;
extern const int PF_FILE;
extern const int PF_IB;
extern const int PF_IEEE802154;
extern const int PF_INET6;
extern const int PF_INET;
extern const int PF_IPX;
extern const int PF_IRDA;
extern const int PF_ISDN;
extern const int PF_IUCV;
extern const int PF_KCM;
extern const int PF_KEY;
extern const int PF_LLC;
extern const int PF_LOCAL;
extern const int PF_MAX;
extern const int PF_MPLS;
extern const int PF_NETBEUI;
extern const int PF_NETLINK;
extern const int PF_NETROM;
extern const int PF_NFC;
extern const int PF_PACKET;
extern const int PF_PHONET;
extern const int PF_PPPOX;
extern const int PF_RDS;
extern const int PF_ROSE;
extern const int PF_ROUTE;
extern const int PF_RXRPC;
extern const int PF_SECURITY;
extern const int PF_SNA;
extern const int PF_TIPC;
extern const int PF_UNIX;
extern const int PF_UNSPEC;
extern const int PF_VSOCK;
extern const int PF_WANPIPE;
extern const int PF_X25;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PF_H_ */
