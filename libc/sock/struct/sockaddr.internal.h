#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR_INTERNAL_H_
#include "libc/mem/alloca.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6-bsd.internal.h"
COSMOPOLITAN_C_START_

struct sockaddr_bsd {
  uint8_t sa_len;    /* « different type */
  uint8_t sa_family; /* « different type */
  char sa_data[14];
};

struct sockaddr_in_bsd {
  uint8_t sin_len;    /* « different type */
  uint8_t sin_family; /* « different type */
  uint16_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
};

struct sockaddr_un_bsd {
  uint8_t sun_len; /* sockaddr len including NUL on freebsd but excluding it on
                      openbsd/xnu */
  uint8_t sun_family;
  char sun_path[108];
};

union sockaddr_storage_bsd {
  struct sockaddr_bsd sa;
  struct sockaddr_in_bsd sin;
  struct sockaddr_in6_bsd sin6;
  struct sockaddr_un_bsd sun;
};

union sockaddr_storage_linux {
  struct sockaddr sa;
  struct sockaddr_in sin;
  struct sockaddr_in6 sin6;
  struct sockaddr_un sun;
};

const char *DescribeSockaddr(char[128], const struct sockaddr *, size_t);
#define DescribeSockaddr(sa, sz) DescribeSockaddr(alloca(128), sa, sz)

void __convert_bsd_to_sockaddr(struct sockaddr_storage *);
void __convert_sockaddr_to_bsd(struct sockaddr_storage *);
uint8_t __get_sockaddr_len(const struct sockaddr_storage *);
void __write_sockaddr(const struct sockaddr_storage *, void *, uint32_t *);

int sockaddr2bsd(const void *, uint32_t, union sockaddr_storage_bsd *,
                 uint32_t *);
void sockaddr2linux(const union sockaddr_storage_bsd *, uint32_t,
                    union sockaddr_storage_linux *, uint32_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR_INTERNAL_H_ */
