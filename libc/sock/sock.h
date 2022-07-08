#ifndef COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#define COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#include "libc/bits/bswap.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/sock/struct/linger.h"
#include "libc/sock/struct/msghdr.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system api » berkeley sockets                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define INET_ADDRSTRLEN 22

#define htons(u16) bswap_16(u16)
#define ntohs(u16) bswap_16(u16)
#define htonl(u32) bswap_32(u32)
#define ntohl(u32) bswap_32(u32)

struct ip_mreq {
  struct in_addr imr_multiaddr; /* IP multicast address of group */
  struct in_addr imr_interface; /* local IP address of interface */
};

/*
 *  Structure used in SIOCGIFCONF request.
 *  Used to retrieve interface configuration
 *  for machine (useful for programs which
 *  must know all networks accessible).
 */
struct ifconf {
  uint64_t ifc_len; /* size of buffer   */
  union {
    char *ifcu_buf;
    struct ifreq *ifcu_req;
  } ifc_ifcu;
};

/* Shortcuts to the ifconf buffer or ifreq array */
#define ifc_buf ifc_ifcu.ifcu_buf /* buffer address   */
#define ifc_req ifc_ifcu.ifcu_req /* array of structures  */

#define IFHWADDRLEN 6
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
    char ifru_pad[24];              /* ifru_map is the largest, just pad */
  } ifr_ifru;
};

#define ifr_name      ifr_ifrn.ifrn_name      /* interface name 	*/
#define ifr_addr      ifr_ifru.ifru_addr      /* address		*/
#define ifr_netmask   ifr_ifru.ifru_netmask   /* netmask		*/
#define ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address	*/
#define ifr_dstaddr   ifr_ifru.ifru_dstaddr   /* destination address	*/
#define ifr_flags     ifr_ifru.ifru_flags     /* flags		*/

#define _IOT_ifreq       _IOT(_IOTS(char), IFNAMSIZ, _IOTS(char), 16, 0, 0)
#define _IOT_ifreq_short _IOT(_IOTS(char), IFNAMSIZ, _IOTS(short), 1, 0, 0)
#define _IOT_ifreq_int   _IOT(_IOTS(char), IFNAMSIZ, _IOTS(int), 1, 0, 0)

const char *inet_ntop(int, const void *, char *, uint32_t);
int inet_aton(const char *, struct in_addr *);
int inet_pton(int, const char *, void *);
uint32_t inet_addr(const char *);
char *inet_ntoa(struct in_addr);
int parseport(const char *);
uint32_t *GetHostIps(void);

int socket(int, int, int);
int accept(int, void *, uint32_t *);
int accept4(int, void *, uint32_t *, int);
int bind(int, const void *, uint32_t);
int connect(int, const void *, uint32_t);
int listen(int, int);
int shutdown(int, int);
int getsockname(int, void *, uint32_t *);
int getpeername(int, void *, uint32_t *);
ssize_t send(int, const void *, size_t, int);
ssize_t recv(int, void *, size_t, int);
ssize_t recvmsg(int, struct msghdr *, int);
ssize_t recvfrom(int, void *, size_t, uint32_t, void *, uint32_t *);
ssize_t sendmsg(int, const struct msghdr *, int);
ssize_t readv(int, const struct iovec *, int);
ssize_t writev(int, const struct iovec *, int);
ssize_t sendfile(int, int, int64_t *, size_t);
int getsockopt(int, int, int, void *, uint32_t *);
int setsockopt(int, int, int, const void *, uint32_t);
int socketpair(int, int, int, int[2]);
int poll(struct pollfd *, uint64_t, int32_t);
int ppoll(struct pollfd *, uint64_t, const struct timespec *,
          const struct sigset *);
ssize_t sendto(int, const void *, size_t, uint32_t, const void *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SOCK_H_ */
