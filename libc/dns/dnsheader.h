#ifndef COSMOPOLITAN_LIBC_DNS_DNSHEADER_H_
#define COSMOPOLITAN_LIBC_DNS_DNSHEADER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct DnsHeader {
  uint16_t id;      /* transaction id */
  uint8_t bf1;      /* bit field 1 */
  uint8_t bf2;      /* bit field 2 */
  uint16_t qdcount; /* question count */
  uint16_t ancount; /* answer count */
  uint16_t nscount; /* nameserver count */
  uint16_t arcount; /* additional record count */
};

void SerializeDnsHeader(uint8_t[restrict 12], const struct DnsHeader *);
void DeserializeDnsHeader(struct DnsHeader *, const uint8_t[restrict 12]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_DNSHEADER_H_ */
