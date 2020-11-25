#ifndef COSMOPOLITAN_LIBC_ALG_BISECTCARLEFT_H_
#define COSMOPOLITAN_LIBC_ALG_BISECTCARLEFT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* TODO: DELETE */

forceinline int32_t bisectcarleft(const int32_t (*cons)[2], size_t count,
                                  const int32_t key) {
  size_t left = 0;
  size_t right = count;
  while (left < right) {
    size_t m = (left + right) >> 1;
    if (cons[m][0] < key) {
      left = m + 1;
    } else {
      right = m;
    }
  }
  if (left && (left == count || cons[left][0] > key)) left--;
  return left;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_BISECTCARLEFT_H_ */
