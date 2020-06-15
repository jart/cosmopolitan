#ifndef COSMOPOLITAN_LIBC_DNS_DNS_H_
#define COSMOPOLITAN_LIBC_DNS_DNS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define DNS_PORT 53
#define DNS_NAME_MAX 253
#define DNS_LABEL_MAX 63

struct sockaddr;
struct sockaddr_in;
struct ResolvConf;

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
const char *eai2str(int);
int dnsnamecmp(const char *, const char *) paramsnonnull();
int pascalifydnsname(uint8_t *, size_t, const char *) paramsnonnull();
int resolvedns(const struct ResolvConf *, int, const char *, struct sockaddr *,
               uint32_t) paramsnonnull();
struct addrinfo *newaddrinfo(uint16_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_DNS_H_ */
