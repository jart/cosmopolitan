#ifndef COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_
#define COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_
COSMOPOLITAN_C_START_

struct DnsQuestion {
  const char *qname;
  uint16_t qtype;
  uint16_t qclass;
};

int SerializeDnsQuestion(uint8_t *, size_t, const struct DnsQuestion *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_DNS_DNSQUESTION_H_ */
