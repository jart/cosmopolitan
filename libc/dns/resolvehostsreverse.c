
#include "libc/alg/alg.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

static int hoststxtcmpaddr(const uint8_t *ip1, const struct HostsTxtEntry *he2) {
  uint32_t v1 = *((uint32_t *)ip1), v2 = *((uint32_t *)he2->ip);
  return (v1 == v2 ? 0 : (v1 > v2 ? 1 : -1));
}

/**
 * Finds name associated with address in HOSTS.TXT table.
 *
 * @param ht can be GetHostsTxt()
 * @param af can be AF_INET
 * @param ip is IP address in binary (sin_addr)
 * @param buf is buffer to store the name
 * @param bufsize is length of buf
 *
 * @return 1 if found, 0 if not found, or -1 w/ errno
 * @error EAFNOSUPPORT
 */
int ResolveHostsReverse(const struct HostsTxt *ht, int af, const uint8_t *ip,
                        char *buf, size_t bufsize) {
  struct HostsTxtEntry *entry;
  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  if (!ht->entries.p) return -1;

  if (ht->sorted_by != HOSTSTXT_SORTEDBYADDR)
    SortHostsTxt(ht, HOSTSTXT_SORTEDBYADDR);

  entry = bsearch(ip, ht->entries.p, ht->entries.i,
                  sizeof(struct HostsTxtEntry), (void *)hoststxtcmpaddr);
  if (entry) {
    strncpy(buf, &ht->strings.p[entry->name], bufsize);
    return 1;
  }
  return 0;
}
