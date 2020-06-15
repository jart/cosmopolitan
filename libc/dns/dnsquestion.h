#ifndef COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_
#define COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_
#include "libc/dns/dns.h"
#include "libc/sysv/errfuns.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct DnsQuestion {
  const char *qname;
  uint16_t qtype;
  uint16_t qclass;
};

/**
 * Serializes DNS question record to wire.
 *
 * @return number of bytes written
 * @see pascalifydnsname()
 */
forceinline int serializednsquestion(uint8_t *buf, size_t size,
                                     struct DnsQuestion dq) {
  int wrote;
  if ((wrote = pascalifydnsname(buf, size, dq.qname)) == -1) return -1;
  if (wrote + 1 + 4 > size) return enospc();
  buf[wrote + 1] = dq.qtype >> 010, buf[wrote + 2] = dq.qtype >> 000;
  buf[wrote + 3] = dq.qclass >> 010, buf[wrote + 4] = dq.qclass >> 000;
  return wrote + 5;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_ */
