#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint32_t kCrc32cTab[256];

void crc32init(uint32_t[hasatleast 256], uint32_t);
uint32_t crc32a(uint32_t, const void *, size_t);
uint32_t crc32c(uint32_t, const void *, size_t);
uint32_t crc32_z(uint32_t, const void *, size_t);
uint32_t crc32c_pure(uint32_t, const void *, size_t) strlenesque _Hide;
uint32_t crc32c_sse42(uint32_t, const void *, size_t) strlenesque _Hide;
uint32_t crc32_pclmul(uint32_t, const void *, size_t) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_ */
