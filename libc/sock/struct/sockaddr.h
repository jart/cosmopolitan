#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR_H_
COSMOPOLITAN_C_START_

struct sockaddr {     /* Linux+NT ABI */
  uint16_t sa_family; /* AF_XXX */
  char sa_data[14];
};

struct in_addr { /* ARPA ABI */
  /* e.g. 127|0<<8|0<<16|1<<24 or inet_pton(AF_INET, "127.0.0.1", &s_addr) */
  uint32_t s_addr;
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

int inet_aton(const char *, struct in_addr *);
char *inet_ntoa(struct in_addr);
int accept(int, struct sockaddr *, uint32_t *);
int accept4(int, struct sockaddr *, uint32_t *, int);
int bind(int, const struct sockaddr *, uint32_t);
int connect(int, const struct sockaddr *, uint32_t);
int getsockname(int, struct sockaddr *, uint32_t *);
int getpeername(int, struct sockaddr *, uint32_t *);
ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *, uint32_t *);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
               uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR_H_ */
