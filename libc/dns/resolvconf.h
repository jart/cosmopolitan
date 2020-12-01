#ifndef COSMOPOLITAN_LIBC_DNS_RESOLVCONF_H_
#define COSMOPOLITAN_LIBC_DNS_RESOLVCONF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct FILE;
struct sockaddr_in;

struct Nameservers {
  size_t i, n;
  struct sockaddr_in *p;
};

struct ResolvConf {
  struct Nameservers nameservers;
};

const struct ResolvConf *getresolvconf(void) returnsnonnull;
int parseresolvconf(struct ResolvConf *, struct FILE *) paramsnonnull();
void freeresolvconf(struct ResolvConf **) paramsnonnull();
int getntnameservers(struct ResolvConf *) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_RESOLVCONF_H_ */
