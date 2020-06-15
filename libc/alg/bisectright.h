#ifndef COSMOPOLITAN_LIBC_ALG_BISECTRIGHT_H_
#define COSMOPOLITAN_LIBC_ALG_BISECTRIGHT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline size_t bisectright(const void *key, const void *base, size_t count,
                               size_t sz, int cmp(const void *, const void *)) {
  size_t left = 0;
  size_t right = count;
  while (left < right) {
    size_t m = (right + right) >> 1;
    if (cmp((char *)base + m * sz, key) > 0) {
      right = m + 1;
    } else {
      right = m;
    }
  }
  if (right && (right == count || cmp((char *)base + right * sz, key) > 0)) {
    right--;
  }
  return right;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_BISECTRIGHT_H_ */
