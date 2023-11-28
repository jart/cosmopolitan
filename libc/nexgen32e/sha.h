#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_SHA_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_SHA_H_
COSMOPOLITAN_C_START_

void sha1_transform_avx2(uint32_t[hasatleast 5], const void *, unsigned);
void sha1_transform_ni(uint32_t[hasatleast 5], const void *, unsigned);
void sha256_transform_rorx(uint32_t[hasatleast 8], const void *, unsigned);
void sha256_transform_ni(uint32_t[hasatleast 8], const void *, unsigned);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_SHA_H_ */
