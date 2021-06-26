#ifndef COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#define COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#include "libc/bits/bswap.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system api » berkeley sockets                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define INET_ADDRSTRLEN 22

#define NI_NUMERICHOST 0x01
#define NI_NUMERICSERV 0x02
#define NI_NOFQDN      0x04
#define NI_NAMEREQD    0x08
#define NI_DGRAM       0x10

#define NI_MAXHOST 0xff
#define NI_MAXSERV 0x20

#define htons(u16) bswap_16(u16)
#define ntohs(u16) bswap_16(u16)
#define htonl(u32) bswap_32(u32)
#define ntohl(u32) bswap_32(u32)

struct in_addr { /* ARPA ABI */
  /* e.g. 127|0<<8|0<<16|1<<24 or inet_pton(AF_INET, "127.0.0.1", &s_addr) */
  uint32_t s_addr;
};

struct sockaddr {     /* Linux+NT ABI */
  uint16_t sa_family; /* AF_XXX */
  char sa_data[14];
};

struct sockaddr_in {   /* Linux+NT ABI */
  uint16_t sin_family; /* AF_XXX */
  uint16_t sin_port;   /* htons(XXX) i.e. big endian */
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
};

struct sockaddr_un {
  uint16_t sun_family; /* AF_UNIX */
  char sun_path[108];  /* path */
};

struct sockaddr_storage {
  union {
    uint16_t ss_family;
    intptr_t __ss_align;
    char __ss_storage[128];
  };
};

struct ip_mreq {
  struct in_addr imr_multiaddr; /* IP multicast address of group */
  struct in_addr imr_interface; /* local IP address of interface */
};

struct linger {     /* Linux+XNU+BSD ABI */
  int32_t l_onoff;  /* on/off */
  int32_t l_linger; /* seconds */
};

struct pollfd {
  int32_t fd;
  int16_t events;
  int16_t revents;
};

struct msghdr {            /* Linux+NT ABI */
  void *msg_name;          /* optional address */
  uint32_t msg_namelen;    /* size of msg_name */
  struct iovec *msg_iov;   /* scatter/gather array */
  uint64_t msg_iovlen;     /* iovec count */
  void *msg_control;       /* credentials and stuff */
  uint64_t msg_controllen; /* size of msg_control */
  uint32_t msg_flags;      /* MSG_XXX */
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

#define ifr_name      ifr_ifrn.ifrn_name /* interface name 	*/
#define ifr_addr      ifr_ifru.ifru_addr /* address		*/
#define ifr_netmask   ifr_ifru.ifru_netmask /* netmask		*/
#define ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address	*/
#define ifr_dstaddr   ifr_ifru.ifru_dstaddr /* destination address	*/
#define ifr_flags     ifr_ifru.ifru_flags /* flags		*/

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

int socket(int, int, int) nodiscard;
int accept(int, void *, uint32_t *) nodiscard;
int accept4(int, void *, uint32_t *, int) nodiscard;
int bind(int, const void *, uint32_t);
int connect(int, const void *, uint32_t);
int listen(int, int);
int shutdown(int, int);
int getsockname(int, void *, uint32_t *) paramsnonnull();
int getpeername(int, void *, uint32_t *) paramsnonnull();
ssize_t send(int, const void *, size_t, int) paramsnonnull();
ssize_t recv(int, void *, size_t, int);
ssize_t recvmsg(int, struct msghdr *, int) paramsnonnull();
ssize_t recvfrom(int, void *, size_t, uint32_t, void *, uint32_t *);
ssize_t sendmsg(int, const struct msghdr *, int) paramsnonnull();
ssize_t readv(int, const struct iovec *, int);
ssize_t writev(int, const struct iovec *, int);
ssize_t sendfile(int, int, int64_t *, size_t);
int getsockopt(int, int, int, void *, uint32_t *) paramsnonnull((5));
int setsockopt(int, int, int, const void *, uint32_t);
int socketpair(int, int, int, int[2]) paramsnonnull();
int poll(struct pollfd *, uint64_t, int32_t) paramsnonnull();
int ppoll(struct pollfd *, uint64_t, const struct timespec *,
          const struct sigset *) paramsnonnull((1, 4));
ssize_t sendto(int, const void *, size_t, uint32_t, const void *, uint32_t)
    paramsnonnull((2));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SOCK_H_ */
