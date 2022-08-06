#ifndef COSMOPOLITAN_LIBC_DNS_HOSTSTXT_H_
#define COSMOPOLITAN_LIBC_DNS_HOSTSTXT_H_
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct HostsTxtEntry {
  uint8_t ip[4];  /* inet_ntop(AF_INET, he->ip, buf, size) */
  uint32_t name;  /* &ht->strings.p[he->name] */
  uint32_t canon; /* &ht->strings.p[he->canon] */
};

struct HostsTxtEntries {
  size_t i, n;
  struct HostsTxtEntry *p;
};

struct HostsTxtStrings {
  size_t i, n;
  char *p;
};

struct HostsTxt {
  struct HostsTxtEntries entries;
  struct HostsTxtStrings strings;
};

const struct HostsTxt *GetHostsTxt(void) returnsnonnull;
void FreeHostsTxt(struct HostsTxt **) paramsnonnull();
int ParseHostsTxt(struct HostsTxt *, FILE *) paramsnonnull();
int ResolveHostsTxt(const struct HostsTxt *, int, const char *,
                    struct sockaddr *, uint32_t, const char **)
    paramsnonnull((1, 3));
int ResolveHostsReverse(const struct HostsTxt *, int, const uint8_t *, char *,
                        size_t) paramsnonnull((1, 3));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_HOSTSTXT_H_ */
