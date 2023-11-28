#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_
COSMOPOLITAN_C_START_

extern const uint32_t kCrc32cTab[256];

void crc32init(uint32_t[hasatleast 256], uint32_t);
uint32_t crc32c(uint32_t, const void *, size_t) nosideeffect;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_ */
