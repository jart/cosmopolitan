#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long IFF_UP;
extern const long IFF_ALLMULTI;
extern const long IFF_AUTOMEDIA;
extern const long IFF_BROADCAST;
extern const long IFF_DEBUG;
extern const long IFF_DYNAMIC;
extern const long IFF_LOOPBACK;
extern const long IFF_MASTER;
extern const long IFF_MULTICAST;
extern const long IFF_NOARP;
extern const long IFF_NOTRAILERS;
extern const long IFF_POINTOPOINT;
extern const long IFF_PORTSEL;
extern const long IFF_PROMISC;
extern const long IFF_RUNNING;
extern const long IFF_SLAVE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define IFF_UP          1
#define IFF_BROADCAST   2
#define IFF_DEBUG       4
#define IFF_LOOPBACK    8
#define IFF_ALLMULTI    IFF_ALLMULTI
#define IFF_AUTOMEDIA   IFF_AUTOMEDIA
#define IFF_DYNAMIC     IFF_DYNAMIC
#define IFF_MASTER      IFF_MASTER
#define IFF_MULTICAST   IFF_MULTICAST
#define IFF_NOARP       IFF_NOARP
#define IFF_NOTRAILERS  IFF_NOTRAILERS
#define IFF_POINTOPOINT IFF_POINTOPOINT
#define IFF_PORTSEL     IFF_PORTSEL
#define IFF_PROMISC     IFF_PROMISC
#define IFF_RUNNING     IFF_RUNNING
#define IFF_SLAVE       IFF_SLAVE

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_ */
