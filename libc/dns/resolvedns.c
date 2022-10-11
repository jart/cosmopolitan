/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/dnsheader.h"
#include "libc/dns/dnsquestion.h"
#include "libc/dns/resolvconf.h"
#include "libc/intrin/bits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"

#define kMsgMax 512

/**
 * Queries Domain Name System for address associated with name.
 *
 * @param resolvconf can be GetResolvConf()
 * @param af can be AF_INET, AF_UNSPEC
 * @param name can be a local or fully-qualified hostname
 * @param addr should point to a struct sockaddr_in; if this function
 *     succeeds, its sin_family and sin_addr fields will be modified
 * @param addrsize is the byte size of addr
 * @return number of matches found, or -1 w/ errno
 * @error EAFNOSUPPORT. ENETDOWN, ENAMETOOLONG, EBADMSG
 */
int ResolveDns(const struct ResolvConf *resolvconf, int af, const char *name,
               struct sockaddr *addr, uint32_t addrsize) {
  int32_t ttl;
  int rc, fd, n;
  struct DnsQuestion q;
  struct DnsHeader h, h2;
  struct sockaddr_in *a4;
  uint8_t *p, *pe, msg[512];
  uint16_t rtype, rclass, rdlength;
  if (addrsize < kMinSockaddr4Size) return einval();
  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  if (!resolvconf->nameservers.i) return 0;
  bzero(&h, sizeof(h));
  rc = ebadmsg();
  h.id = _rand64();
  h.bf1 = 1; /* recursion desired */
  h.qdcount = 1;
  q.qname = name;
  q.qtype = DNS_TYPE_A;
  q.qclass = DNS_CLASS_IN;
  bzero(msg, sizeof(msg));
  SerializeDnsHeader(msg, &h);
  if ((n = SerializeDnsQuestion(msg + 12, 500, &q)) == -1) return -1;
  if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) return -1;
  if (sendto(fd, msg, 12 + n, 0, (struct sockaddr *)resolvconf->nameservers.p,
             sizeof(*resolvconf->nameservers.p)) == 12 + n &&
      (n = read(fd, msg, 512)) >= 12) {
    DeserializeDnsHeader(&h2, msg);
    if (h2.id == h.id) {
      rc = 0;
      p = msg + 12;
      pe = msg + n;
      while (p < pe && h2.qdcount--) {
        p += strnlen((char *)p, pe - p) + 1 + 4;
      }
      while (p < pe && h2.ancount--) {
        if ((p[0] & 0xc0) == 0xc0) { /* name pointer */
          p += 2;
        } else {
          p += strnlen((char *)p, pe - p) + 1;
        }
        if (p + 10 <= pe) {
          rtype = READ16BE(p);
          rclass = READ16BE(p + 2);
          ttl = READ32BE(p + 4);
          rdlength = READ16BE(p + 8);
          if (p + 10 + rdlength <= pe && rdlength == 4 &&
              rclass == DNS_CLASS_IN && rtype == DNS_TYPE_A) {
            rc = 1;
            a4 = (struct sockaddr_in *)addr;
            a4->sin_family = AF_INET;
            memcpy(&a4->sin_addr.s_addr, p + 10, 4);
            break;
          }
          p += 10 + rdlength;
        }
      }
    }
  }
  close(fd);
  return rc;
}
