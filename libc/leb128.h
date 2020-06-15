#ifndef COSMOPOLITAN_LIBC_LEB128_H_
#define COSMOPOLITAN_LIBC_LEB128_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Decodes a GNU-style varint from a buffer.
 *
 * The GNU Assembler is able to encode numbers this way, since it's used
 * by the DWARF debug format.
 */
forceinline int unsleb128(const void *buf, size_t size, int64_t *out) {
  const unsigned char *p = (const unsigned char *)buf;
  const unsigned char *pe = (const unsigned char *)buf + size;
  int64_t res = 0;
  int bits = 0;
  unsigned char c;
  do {
    if (size && p == pe) return -1;
    c = *p++;
    res |= (int64_t)(c & 0x7f) << bits;
    bits += 7;
  } while (c & 0x80);
  if ((c & 0x40) != 0) res |= -1ULL << bits;
  if (out) *out = res;
  return p - (const unsigned char *)buf;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LEB128_H_ */
