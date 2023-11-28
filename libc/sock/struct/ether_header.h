#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_ETHER_HEADER_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_ETHER_HEADER_H_

#define ETH_ALEN 6

COSMOPOLITAN_C_START_

struct ether_addr {
  uint8_t ether_addr_octet[ETH_ALEN];
};

struct ether_header {
  uint8_t ether_dhost[ETH_ALEN];
  uint8_t ether_shost[ETH_ALEN];
  uint16_t ether_type;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_ETHER_HEADER_H_ */
