
#include "libc/alg/alg.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

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
  struct HostsTxtEntry *entry = NULL;
  uint32_t v1, v2;

  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  if (!ht->entries.p || !buf || bufsize == 0) return -1;

  v1 = *((uint32_t *)ip);
  for (size_t j = 0; j < ht->entries.i; j++) {
    v2 = *((uint32_t *)ht->entries.p[j].ip);
    if (v1 == v2) {
      entry = &(ht->entries.p[j]);
      break;
    }
  }

  if (entry) {
    strncpy(buf, &ht->strings.p[entry->name], bufsize);
    return 1;
  }
  return 0;
}
