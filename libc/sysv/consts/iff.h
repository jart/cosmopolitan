#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_
#include "libc/runtime/symbolic.h"

#define IFF_ALLMULTI SYMBOLIC(IFF_ALLMULTI)
#define IFF_AUTOMEDIA SYMBOLIC(IFF_AUTOMEDIA)
#define IFF_BROADCAST SYMBOLIC(IFF_BROADCAST)
#define IFF_DEBUG SYMBOLIC(IFF_DEBUG)
#define IFF_DYNAMIC SYMBOLIC(IFF_DYNAMIC)
#define IFF_LOOPBACK SYMBOLIC(IFF_LOOPBACK)
#define IFF_MASTER SYMBOLIC(IFF_MASTER)
#define IFF_MULTICAST SYMBOLIC(IFF_MULTICAST)
#define IFF_NOARP SYMBOLIC(IFF_NOARP)
#define IFF_NOTRAILERS SYMBOLIC(IFF_NOTRAILERS)
#define IFF_POINTOPOINT SYMBOLIC(IFF_POINTOPOINT)
#define IFF_PORTSEL SYMBOLIC(IFF_PORTSEL)
#define IFF_PROMISC SYMBOLIC(IFF_PROMISC)
#define IFF_RUNNING SYMBOLIC(IFF_RUNNING)
#define IFF_SLAVE SYMBOLIC(IFF_SLAVE)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long IFF_ALLMULTI;
hidden extern const long IFF_AUTOMEDIA;
hidden extern const long IFF_BROADCAST;
hidden extern const long IFF_DEBUG;
hidden extern const long IFF_DYNAMIC;
hidden extern const long IFF_LOOPBACK;
hidden extern const long IFF_MASTER;
hidden extern const long IFF_MULTICAST;
hidden extern const long IFF_NOARP;
hidden extern const long IFF_NOTRAILERS;
hidden extern const long IFF_POINTOPOINT;
hidden extern const long IFF_PORTSEL;
hidden extern const long IFF_PROMISC;
hidden extern const long IFF_RUNNING;
hidden extern const long IFF_SLAVE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_ */
