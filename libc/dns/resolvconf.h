#ifndef COSMOPOLITAN_LIBC_DNS_RESOLVCONF_H_
#define COSMOPOLITAN_LIBC_DNS_RESOLVCONF_H_
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Nameservers {
  size_t i, n;
  struct sockaddr_in *p;
};

struct ResolvConf {
  struct Nameservers nameservers;
};

const struct ResolvConf *GetResolvConf(void) returnsnonnull;
int ParseResolvConf(struct ResolvConf *, struct FILE *) paramsnonnull();
void FreeResolvConf(struct ResolvConf **) paramsnonnull();
int GetNtNameServers(struct ResolvConf *) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_RESOLVCONF_H_ */
