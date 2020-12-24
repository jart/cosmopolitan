#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void crc32init(uint32_t[hasatleast 256], uint32_t);
uint32_t crc32_z(uint32_t, const void *, size_t);
extern uint32_t (*const crc32c)(uint32_t, const void *, size_t);
uint32_t crc32c$pure(uint32_t, const void *, size_t) strlenesque hidden;
uint32_t crc32c$sse42(uint32_t, const void *, size_t) strlenesque hidden;
uint32_t crc32$pclmul(uint32_t, const void *, size_t) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_CRC32_H_ */
