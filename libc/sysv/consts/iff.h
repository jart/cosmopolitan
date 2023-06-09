#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int IFF_UP;
extern const int IFF_ALLMULTI;
extern const int IFF_AUTOMEDIA;
extern const int IFF_BROADCAST;
extern const int IFF_DEBUG;
extern const int IFF_DYNAMIC;
extern const int IFF_LOOPBACK;
extern const int IFF_MASTER;
extern const int IFF_MULTICAST;
extern const int IFF_NOARP;
extern const int IFF_NOTRAILERS;
extern const int IFF_POINTOPOINT;
extern const int IFF_PORTSEL;
extern const int IFF_PROMISC;
extern const int IFF_RUNNING;
extern const int IFF_SLAVE;

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

#define __tmpcosmo_IFF_ALLMULTI    -15824140
#define __tmpcosmo_IFF_AUTOMEDIA   -15823962
#define __tmpcosmo_IFF_DYNAMIC     -15823848
#define __tmpcosmo_IFF_MASTER      -15823900
#define __tmpcosmo_IFF_MULTICAST   -15824000
#define __tmpcosmo_IFF_NOARP       -15823802
#define __tmpcosmo_IFF_NOTRAILERS  -15824130
#define __tmpcosmo_IFF_POINTOPOINT -15824138
#define __tmpcosmo_IFF_PORTSEL     -15824150
#define __tmpcosmo_IFF_PROMISC     -15824010
#define __tmpcosmo_IFF_RUNNING     -15824080
#define __tmpcosmo_IFF_SLAVE       -15824022

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IFF_H_ */
