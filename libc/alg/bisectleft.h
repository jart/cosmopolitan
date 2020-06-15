#ifndef COSMOPOLITAN_LIBC_ALG_BISECTLEFT_H_
#define COSMOPOLITAN_LIBC_ALG_BISECTLEFT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline size_t bisectleft(const void *key, const void *base, size_t count,
                              size_t sz, int cmp(const void *, const void *)) {
  size_t m, l, r;
  l = 0;
  r = count;
  while (l < r) {
    m = (l + r) >> 1;
    if (cmp((char *)base + m * sz, key) < 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  if (l && (l == count || cmp((char *)base + l * sz, key) > 0)) {
    l--;
  }
  return l;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_BISECTLEFT_H_ */
