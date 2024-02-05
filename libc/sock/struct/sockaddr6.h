#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SOCKADDR6_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SOCKADDR6_H_

struct in6_addr {
  union {
    uint8_t s6_addr[16];
    uint16_t s6_addr16[8];
    uint32_t s6_addr32[4];
  };
};

struct sockaddr_in6 { /* Linux+NT ABI */
  uint16_t sin6_family;
  uint16_t sin6_port;
  uint32_t sin6_flowinfo;
  struct in6_addr sin6_addr;
  uint32_t sin6_scope_id; /* rfc2553 */
};

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SOCKADDR6_H_ */
