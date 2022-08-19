#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_

static inline uint8_t Read8(const uint8_t *p) {
  return p[0];
}

static inline void Write8(uint8_t *p, uint8_t v) {
  *p = v;
}

static inline uint16_t Read16(const uint8_t *p) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint16_t v;
  __builtin_memcpy(&v, p, sizeof(v));
  return v;
#else
  return p[1] << 8 | p[0];
#endif
}

static inline void Write16(uint8_t *p, uint16_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  __builtin_memcpy(p, &v, sizeof(v));
#else
  p[0] = (0x00FF & v) >> 000;
  p[1] = (0xFF00 & v) >> 010;
#endif
}

static inline uint32_t Read32(const uint8_t *p) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint32_t v;
  __builtin_memcpy(&v, p, sizeof(v));
  return v;
#else
  return ((uint32_t)p[0] << 000 | (uint32_t)p[1] << 010 |
          (uint32_t)p[2] << 020 | (uint32_t)p[3] << 030);
#endif
}

static inline void Write32(uint8_t *p, uint32_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  __builtin_memcpy(p, &v, sizeof(v));
#else
  p[0] = (0x000000FF & v) >> 000;
  p[1] = (0x0000FF00 & v) >> 010;
  p[2] = (0x00FF0000 & v) >> 020;
  p[3] = (0xFF000000 & v) >> 030;
#endif
}

static inline uint64_t Read64(const uint8_t *p) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint64_t v;
  __builtin_memcpy(&v, p, sizeof(v));
  return v;
#else
  return ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
          (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
          (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050 |
          (uint64_t)p[6] << 060 | (uint64_t)p[7] << 070);
#endif
}

static inline void Write64(uint8_t *p, uint64_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  __builtin_memcpy(p, &v, sizeof(v));
#else
  p[0] = (0x00000000000000FF & v) >> 000;
  p[1] = (0x000000000000FF00 & v) >> 010;
  p[2] = (0x0000000000FF0000 & v) >> 020;
  p[3] = (0x00000000FF000000 & v) >> 030;
  p[4] = (0x000000FF00000000 & v) >> 040;
  p[5] = (0x0000FF0000000000 & v) >> 050;
  p[6] = (0x00FF000000000000 & v) >> 060;
  p[7] = (0xFF00000000000000 & v) >> 070;
#endif
}

#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_ */
