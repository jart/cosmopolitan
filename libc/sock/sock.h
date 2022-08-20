#ifndef COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#define COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#include "libc/intrin/bswap.h"
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

#define IFHWADDRLEN 6

const char *inet_ntop(int, const void *, char *, uint32_t);
int inet_pton(int, const char *, void *);
uint32_t inet_addr(const char *);
int parseport(const char *);
uint32_t *GetHostIps(void);

int nointernet(void);
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
ssize_t recvfrom(int, void *, size_t, uint32_t, void *, uint32_t *);
ssize_t sendfile(int, int, int64_t *, size_t);
int getsockopt(int, int, int, void *, uint32_t *);
int setsockopt(int, int, int, const void *, uint32_t);
int socketpair(int, int, int, int[2]);
ssize_t sendto(int, const void *, size_t, uint32_t, const void *, uint32_t);
int sockatmark(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SOCK_H_ */
