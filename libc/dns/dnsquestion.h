#ifndef COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_
#define COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct DnsQuestion {
  const char *qname;
  uint16_t qtype;
  uint16_t qclass;
};

int serializednsquestion(uint8_t *, size_t, struct DnsQuestion);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_ */
