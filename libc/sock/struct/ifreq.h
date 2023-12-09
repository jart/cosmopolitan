#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_IFREQ_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_IFREQ_H_
#include "libc/sock/struct/sockaddr.h"
COSMOPOLITAN_C_START_

#define IF_NAMESIZE 16
#define IFNAMSIZ    IF_NAMESIZE

struct ifreq {
  union {
    char ifrn_name[IFNAMSIZ]; /* Interface name, e.g. "en0".  */
  } ifr_ifrn;
  union {
    struct sockaddr ifru_addr;      /* SIOCGIFADDR */
    struct sockaddr ifru_dstaddr;   /* SIOCGIFDSTADDR */
    struct sockaddr ifru_netmask;   /* SIOCGIFNETMASK */
    struct sockaddr ifru_broadaddr; /* SIOCGIFBRDADDR */
    short ifru_flags;               /* SIOCGIFFLAGS */
    int ifru_ivalue;                /* todo(jart) */
    char ifru_pad[24];              /* ifru_map is the largest, just pad */
  } ifr_ifru;
};

#define ifr_name      ifr_ifrn.ifrn_name      /* interface name */
#define ifr_addr      ifr_ifru.ifru_addr      /* address */
#define ifr_netmask   ifr_ifru.ifru_netmask   /* netmask */
#define ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#define ifr_dstaddr   ifr_ifru.ifru_dstaddr   /* destination address */
#define ifr_flags     ifr_ifru.ifru_flags     /* flags */
#define ifr_ifindex   ifr_ifru.ifru_ivalue

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_IFREQ_H_ */
