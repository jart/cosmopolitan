#ifndef COSMOPOLITAN_LIBC_STR_STRCMP8TO16I_H_
#define COSMOPOLITAN_LIBC_STR_STRCMP8TO16I_H_
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline int strcmp8to16i(const char *s1, const char16_t *s2, size_t n,
                             unsigned xlat(unsigned)) {
  int res = 0;
  if (n) {
    do {
      unsigned i, j;
      wint_t wc1, wc2;
      i = tpdecode(s1, &wc1);
      j = getutf16(s2, &wc2);
      s1 += ABS(i);
      s2 += ABS(j);
      if ((res = xlat(wc1) - xlat(wc2)) || !wc1) break;
    } while (n == -1ul || --n);
  }
  return res;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_STRCMP8TO16I_H_ */
