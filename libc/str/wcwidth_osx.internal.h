#ifndef COSMOPOLITAN_LIBC_STR_WCWIDTH_OSX_H_
#define COSMOPOLITAN_LIBC_STR_WCWIDTH_OSX_H_
COSMOPOLITAN_C_START_

extern const uint32_t kWcwidthOsx[591];
extern const uint8_t kWcwidthOsxIndex1[136];
extern const uint16_t kWcwidthOsxIndex2[228];
extern const uint32_t kWcwidthOsxIndex3[917];

static inline int _wcwidth_osx(uint32_t codePoint) {
  uint32_t a, b, c, d;
  a = kWcwidthOsxIndex1[codePoint >> 13];
  b = kWcwidthOsxIndex2[a + ((codePoint >> 9) & 0xf)];
  c = kWcwidthOsxIndex3[b + ((codePoint >> 5) & 0xf)];
  d = c + (codePoint & 0x1f);
  return (kWcwidthOsx[d >> 4] >> ((d & 0xf) << 1)) & 3;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_WCWIDTH_OSX_H_ */
