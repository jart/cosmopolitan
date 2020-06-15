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

int serializednsheader(uint8_t *buf, size_t size,
                       const struct DnsHeader header);
int deserializednsheader(struct DnsHeader *header, const uint8_t *buf,
                         size_t size);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_DNSHEADER_H_ */
