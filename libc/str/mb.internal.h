#ifndef COSMOPOLITAN_LIBC_STR_MB_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STR_MB_INTERNAL_H_
COSMOPOLITAN_C_START_

#define SA             0xc2u
#define SB             0xf4u
#define CODEUNIT(c)    (0xdfff & (signed char)(c))
#define IS_CODEUNIT(c) ((unsigned)(c)-0xdf80 < 0x80)
#define R(a, b)        ((uint32_t)((a == 0x80 ? 0x40u - b : 0u - a) << 23))
#define FAILSTATE      R(0x80, 0x80)
#define OOB(c, b) \
  (((((b) >> 3) - 0x10) | (((b) >> 3) + ((int32_t)(c) >> 26))) & ~7)

extern const uint32_t kMbBittab[51];

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_MB_INTERNAL_H_ */
