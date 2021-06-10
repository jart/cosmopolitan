/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
│                                                                              │
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
 * Performs reverse DNS lookup with IP address.
 *
 * @param resolvconf can be GetResolvConf()
 * @param af can be AF_INET, AF_UNSPEC
 * @param name is a reversed IP address string ending with .in-addr.arpa
 * @param buf to store the obtained hostname if any
 * @param bufsize is size of buf
 *
 * @return 0 on success, or -1 w/ errno
 * @error EAFNOSUPPORT, ENETDOWN, ENAMETOOLONG, EBADMSG
 */
int ResolveDnsReverse(const struct ResolvConf *resolvconf, int af,
                      const char *name, char *buf, size_t bufsize) {
  int rc, fd, n;
  struct DnsQuestion q;
  struct DnsHeader h, h2;
  uint8_t *p, *pe, msg[512];
  uint16_t rtype, rclass, rdlength;

  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  if (!resolvconf->nameservers.i) return 0;
  memset(&h, 0, sizeof(h));
  rc = ebadmsg();
  h.id = rand32();
  h.bf1 = 1; /* recursion desired */
  h.qdcount = 1;
  q.qname = name;
  q.qtype = DNS_TYPE_PTR;
  q.qclass = DNS_CLASS_IN;
  memset(msg, 0, sizeof(msg));
  SerializeDnsHeader(msg, &h);

  if ((n = SerializeDnsQuestion(msg + 12, 500, &q)) == -1) return -1;
  if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) return -1;
  if (sendto(fd, msg, 12 + n, 0, resolvconf->nameservers.p,
             sizeof(*resolvconf->nameservers.p)) == 12 + n &&
      (n = read(fd, msg, 512)) >= 12) {
    DeserializeDnsHeader(&h2, msg);
    if (h2.id == h.id) {
      rc = 0;
      if (h2.ancount) {
        p = msg + 12;
        pe = msg + n;
        while (p < pe && h2.qdcount) {
          p += strnlen((char *)p, pe - p) + 1 + 4;
          h2.qdcount--;
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
            rdlength = READ16BE(p), p += 2;

            if (p + rdlength <= pe && rtype == DNS_TYPE_PTR &&
                rclass == DNS_CLASS_IN) {
              if (strnlen((char *)p, pe - p) + 1 > bufsize)
                rc = -1;
              else {
                /* domain name starts with a letter */
                for (; !isalnum((char)(*p)) && p < pe; p++) rdlength--;
                for (char *tmp = (char *)p; rdlength > 0 && *tmp != '\0';
                     tmp++) {
                  /* each label is alphanumeric or hyphen
                   * any other character is assumed separator */
                  if (!isalnum(*tmp) && *tmp != '-') *tmp = '.';
                  rdlength--;
                }
                strcpy(buf, (char *)p);
              }
            } else
              rc = -1;
          }
        }
      }
    }
  }
  close(fd);
  return rc;
}
