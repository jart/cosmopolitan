#ifndef COSMOPOLITAN_LIBC_DNS_HOSTSTXT_H_
#define COSMOPOLITAN_LIBC_DNS_HOSTSTXT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct FILE;
struct sockaddr;

struct HostsTxtEntry {
  unsigned char ip[4]; /* inet_ntop(AF_INET, he->ip, buf, size) */
  uint32_t name;       /* &ht->strings.p[he->name] */
  uint32_t canon;      /* &ht->strings.p[he->canon] */
};

struct HostsTxtEntries {
  size_t i;
  size_t n;
  struct HostsTxtEntry *p;
};

struct HostsTxtStrings {
  size_t i;
  size_t n;
  char *p;
};

struct HostsTxt {
  struct HostsTxtEntries entries;
  struct HostsTxtStrings strings;
};

const struct HostsTxt *gethoststxt(void) returnsnonnull;
void freehoststxt(struct HostsTxt **) paramsnonnull();
int parsehoststxt(struct HostsTxt *, struct FILE *) paramsnonnull();
void sorthoststxt(struct HostsTxt *) paramsnonnull();
int resolvehoststxt(const struct HostsTxt *, int, const char *,
                    struct sockaddr *, uint32_t, const char **)
    paramsnonnull((1, 3));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_HOSTSTXT_H_ */
