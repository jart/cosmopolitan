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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/dnsheader.h"
#include "libc/dns/dnsquestion.h"
#include "libc/dns/resolvconf.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"

#define kMsgMax 512

/**
 * Queries Domain Name System for address associated with name.
 *
 * @param resolvconf can be getresolvconf()
 * @param af can be AF_INET, AF_UNSPEC
 * @param name can be a local or fully-qualified hostname
 * @param addr should point to a struct sockaddr_in; if this function
 *     succeeds, its sin_family and sin_addr fields will be modified
 * @param addrsize is the byte size of addr
 * @return number of matches found, or -1 w/ errno
 * @error EAFNOSUPPORT. ENETDOWN, ENAMETOOLONG, EBADMSG
 */
int resolvedns(const struct ResolvConf *resolvconf, int af, const char *name,
               struct sockaddr *addr, uint32_t addrsize) {
  size_t msgsize;
  int res, fd, rc, rc2;
  struct sockaddr_in *addr4;
  struct DnsQuestion question;
  uint16_t rtype, rclass, rdlength;
  uint8_t *p, *pe, *outmsg, *inmsg;
  struct DnsHeader header, response;
  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  if (!resolvconf->nameservers.i) return 0;
  memset(&header, 0, sizeof(header));
  header.id = rand32();
  header.bf1 = 1; /* recursion desired */
  header.qdcount = 1;
  question.qname = name;
  question.qtype = DNS_TYPE_A;
  question.qclass = DNS_CLASS_IN;
  res = -1;
  if ((outmsg = malloc(kMsgMax)) && (inmsg = malloc(kMsgMax)) &&
      (rc = serializednsheader(outmsg, kMsgMax, header)) != -1 &&
      (rc2 = serializednsquestion(outmsg + rc, kMsgMax - rc, question)) != -1) {
    msgsize = rc + rc2;
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1 &&
        sendto(fd, outmsg, msgsize, 0, (void *)&resolvconf->nameservers.p[0],
               sizeof(resolvconf->nameservers.p[0])) == msgsize) {
      if ((rc = recv(fd, inmsg, kMsgMax, 0)) != -1 &&
          (rc2 = deserializednsheader(&response, inmsg, rc)) != -1 &&
          response.id == header.id) {
        res = 0;
        if (response.ancount) {
          p = inmsg + rc2;
          pe = inmsg + rc;
          while (p < pe && response.qdcount) {
            p += strnlen((char *)p, pe - p) + 1 + 4;
            response.qdcount--;
          }
          if (p + 1 < pe) {
            if ((p[0] & 0b11000000) == 0b11000000) { /* name pointer */
              p += 2;
            } else {
              p += strnlen((char *)p, pe - p) + 1;
            }
            if (p + 2 + 2 + 4 + 2 < pe) {
              rtype = READ16BE(p), p += 2;
              rclass = READ16BE(p), p += 2;
              /* ttl */ p += 4;
              rdlength = read16be(p), p += 2;
              if (p + rdlength <= pe && rdlength == 4 &&
                  (rtype == DNS_TYPE_A && rclass == DNS_CLASS_IN)) {
                res = 1;
                if (addrsize) {
                  if (addrsize >= kMinSockaddr4Size) {
                    addr4 = (struct sockaddr_in *)addr;
                    addr4->sin_family = AF_INET;
                    memcpy(&addr4->sin_addr.s_addr, p, 4);
                  } else {
                    res = einval();
                  }
                }
              }
            }
          }
        }
      }
    }
    res |= close(fd);
  }
  free(outmsg);
  return res;
}
