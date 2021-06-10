#ifndef COSMOPOLITAN_LIBC_DNS_DNS_H_
#define COSMOPOLITAN_LIBC_DNS_DNS_H_
#include "libc/calls/weirdtypes.h"
#include "libc/dns/resolvconf.h"
#include "libc/sock/sock.h"

#define DNS_PORT      53
#define DNS_NAME_MAX  253
#define DNS_LABEL_MAX 63

#define EAI_SUCCESS     0
#define EAI_BADFLAGS    -1
#define EAI_NONAME      -2
#define EAI_AGAIN       -3
#define EAI_FAIL        -4
#define EAI_NODATA      -5
#define EAI_FAMILY      -6
#define EAI_SOCKTYPE    -7
#define EAI_SERVICE     -8
#define EAI_ADDRFAMILY  -9
#define EAI_MEMORY      -10
#define EAI_OVERFLOW    -12
#define EAI_SYSTEM      -11
#define EAI_ALLDONE     -103
#define EAI_CANCELED    -101
#define EAI_IDN_ENCODE  -105
#define EAI_INPROGRESS  -100
#define EAI_INTR        -104
#define EAI_NOTCANCELED -102

/* AI_* conforms to NT ABI */
#define AI_PASSIVE     1
#define AI_CANONNAME   2
#define AI_NUMERICHOST 4
#define AI_NUMERICSERV 8
#define AI_ALL         0x0100
#define AI_ADDRCONFIG  0x0400
#define AI_V4MAPPED    0x0800

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct addrinfo {
  int32_t ai_flags;    /* AI_XXX */
  int32_t ai_family;   /* AF_INET */
  int32_t ai_socktype; /* SOCK_XXX */
  int32_t ai_protocol; /* IPPROTO_XXX */
  uint32_t ai_addrlen;
  union {
    struct sockaddr *ai_addr;
    struct sockaddr_in *ai_addr4;
  };
  char *ai_canonname /*[DNS_NAME_MAX + 1]*/;
  struct addrinfo *ai_next;
};

int getaddrinfo(const char *, const char *, const struct addrinfo *,
                struct addrinfo **) paramsnonnull((4));
int freeaddrinfo(struct addrinfo *);
int getnameinfo(const struct sockaddr *, socklen_t, char *, socklen_t, char *,
                socklen_t, int);
const char *gai_strerror(int);
int CompareDnsNames(const char *, const char *) paramsnonnull();
int PascalifyDnsName(uint8_t *, size_t, const char *) paramsnonnull();
int ResolveDns(const struct ResolvConf *, int, const char *, struct sockaddr *,
               uint32_t) paramsnonnull();
int ResolveDnsReverse(const struct ResolvConf *resolvconf, int, const char *,
                      char *, size_t) paramsnonnull();
struct addrinfo *newaddrinfo(uint16_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_DNS_H_ */
