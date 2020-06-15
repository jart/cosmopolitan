#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_
#include "libc/runtime/symbolic.h"

#define SOL_AAL SYMBOLIC(SOL_AAL)
#define SOL_ALG SYMBOLIC(SOL_ALG)
#define SOL_ATM SYMBOLIC(SOL_ATM)
#define SOL_BLUETOOTH SYMBOLIC(SOL_BLUETOOTH)
#define SOL_CAIF SYMBOLIC(SOL_CAIF)
#define SOL_DCCP SYMBOLIC(SOL_DCCP)
#define SOL_DECNET SYMBOLIC(SOL_DECNET)
#define SOL_ICMPV6 SYMBOLIC(SOL_ICMPV6)
#define SOL_IP SYMBOLIC(SOL_IP)
#define SOL_IPV6 SYMBOLIC(SOL_IPV6)
#define SOL_IRDA SYMBOLIC(SOL_IRDA)
#define SOL_IUCV SYMBOLIC(SOL_IUCV)
#define SOL_KCM SYMBOLIC(SOL_KCM)
#define SOL_LLC SYMBOLIC(SOL_LLC)
#define SOL_NETBEUI SYMBOLIC(SOL_NETBEUI)
#define SOL_NETLINK SYMBOLIC(SOL_NETLINK)
#define SOL_NFC SYMBOLIC(SOL_NFC)
#define SOL_PACKET SYMBOLIC(SOL_PACKET)
#define SOL_PNPIPE SYMBOLIC(SOL_PNPIPE)
#define SOL_PPPOL2TP SYMBOLIC(SOL_PPPOL2TP)
#define SOL_RAW SYMBOLIC(SOL_RAW)
#define SOL_RDS SYMBOLIC(SOL_RDS)
#define SOL_RXRPC SYMBOLIC(SOL_RXRPC)
#define SOL_SOCKET SYMBOLIC(SOL_SOCKET)
#define SOL_TCP SYMBOLIC(SOL_TCP)
#define SOL_TIPC SYMBOLIC(SOL_TIPC)
#define SOL_UDP SYMBOLIC(SOL_UDP)
#define SOL_X25 SYMBOLIC(SOL_X25)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long SOL_AAL;
hidden extern const long SOL_ALG;
hidden extern const long SOL_ATM;
hidden extern const long SOL_BLUETOOTH;
hidden extern const long SOL_CAIF;
hidden extern const long SOL_DCCP;
hidden extern const long SOL_DECNET;
hidden extern const long SOL_ICMPV6;
hidden extern const long SOL_IP;
hidden extern const long SOL_IPV6;
hidden extern const long SOL_IRDA;
hidden extern const long SOL_IUCV;
hidden extern const long SOL_KCM;
hidden extern const long SOL_LLC;
hidden extern const long SOL_NETBEUI;
hidden extern const long SOL_NETLINK;
hidden extern const long SOL_NFC;
hidden extern const long SOL_PACKET;
hidden extern const long SOL_PNPIPE;
hidden extern const long SOL_PPPOL2TP;
hidden extern const long SOL_RAW;
hidden extern const long SOL_RDS;
hidden extern const long SOL_RXRPC;
hidden extern const long SOL_SOCKET;
hidden extern const long SOL_TCP;
hidden extern const long SOL_TIPC;
hidden extern const long SOL_UDP;
hidden extern const long SOL_X25;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SOL_H_ */
