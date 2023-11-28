#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_NEXGEN32E_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_NEXGEN32E_H_
COSMOPOLITAN_C_START_

extern long kHalfCache3;
extern const uint64_t kTens[20];
extern const uint32_t kSha256[64];
extern const uint64_t kSha512[80];
extern const unsigned char kTensIndex[64];

void CheckStackIsAligned(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_NEXGEN32E_H_ */
