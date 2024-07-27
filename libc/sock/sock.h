#ifndef COSMOPOLITAN_LIBC_SOCK_SOCK_H_
#define COSMOPOLITAN_LIBC_SOCK_SOCK_H_
COSMOPOLITAN_C_START_

#define INET_ADDRSTRLEN 22
#define IFHWADDRLEN     6

libcesque uint16_t htons(uint16_t) pureconst;
libcesque uint16_t ntohs(uint16_t) pureconst;
libcesque uint32_t htonl(uint32_t) pureconst;
libcesque uint32_t ntohl(uint32_t) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#endif

/* clang-format off */
const char *inet_ntop(int, const void *, char *, uint32_t) libcesque;
int inet_pton(int, const char *, void *) libcesque;
uint32_t inet_addr(const char *) libcesque;
libcesque uint32_t *GetHostIps(void) __wur;

int socket(int, int, int) libcesque;
int listen(int, int) libcesque __fd_arg(1);
int shutdown(int, int) libcesque __fd_arg(1);
ssize_t send(int, const void *, size_t, int) libcesque __fd_arg(1) __read_only(2, 3);
ssize_t recv(int, void *, size_t, int) libcesque __fd_arg(1) __write_only(2, 3);
ssize_t sendfile(int, int, int64_t *, size_t) libcesque __fd_arg(1) __fd_arg(2) __read_write(3);
int getsockopt(int, int, int, void *, uint32_t *) libcesque __fd_arg(1);
int setsockopt(int, int, int, const void *, uint32_t) libcesque __fd_arg(1);
int socketpair(int, int, int, int[2]) libcesque;
int sockatmark(int) libcesque __fd_arg(1);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_SOCK_H_ */
