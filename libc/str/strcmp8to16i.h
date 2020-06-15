#ifndef COSMOPOLITAN_LIBC_STR_STRCMP8TO16I_H_
#define COSMOPOLITAN_LIBC_STR_STRCMP8TO16I_H_
#include "libc/conv/conv.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline int strcmp8to16i(const char *s1, const char16_t *s2, size_t n,
                             unsigned xlat(unsigned)) {
  int res = 0;
  if (n) {
    do {
      wint_t wc1, wc2;
      s1 += abs(tpdecode(s1, &wc1));
      s2 += abs(getutf16(s2, &wc2));
      if ((res = xlat(wc1) - xlat(wc2)) || !wc1) break;
    } while (n == -1ul || --n);
  }
  return res;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_STRCMP8TO16I_H_ */
