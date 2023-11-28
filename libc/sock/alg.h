#ifndef COSMOPOLITAN_LIBC_SOCK_ALG_H_
#define COSMOPOLITAN_LIBC_SOCK_ALG_H_
COSMOPOLITAN_C_START_

struct sockaddr_alg {
  uint16_t salg_family;
  uint8_t salg_type[14];
  uint32_t salg_feat;
  uint32_t salg_mask;
  uint8_t salg_name[64];
};

struct sockaddr_alg_new {
  uint16_t salg_family;
  uint8_t salg_type[14];
  uint32_t salg_feat;
  uint32_t salg_mask;
  uint8_t salg_name[]; /* Linux v4.12+ */
};

struct af_alg_iv {
  uint32_t ivlen;
  uint8_t iv[0];
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_ALG_H_ */
