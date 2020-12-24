#ifndef COSMOPOLITAN_LIBC_STR_TPDECODECB_H_
#define COSMOPOLITAN_LIBC_STR_TPDECODECB_H_
#include "libc/nexgen32e/bsr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/* TODO(jart): DELETE? */

/**
 * Generic Thompson-Pike Varint Decoder.
 * @return number of bytes successfully consumed or -1 w/ errno
 * @note synchronization is performed
 * @todo delete
 */
forceinline int tpdecodecb(wint_t *out, int first,
                           int get(void *arg, uint32_t i), void *arg) {
  uint32_t wc, cb, need, msb, j, i = 1;
  if (__builtin_expect((wc = first) == -1, 0)) return -1;
  while (__builtin_expect((wc & 0b11000000) == 0b10000000, 0)) {
    if ((wc = get(arg, i++)) == -1) return -1;
  }
  if (__builtin_expect(!(0 <= wc && wc <= 0x7F), 0)) {
    msb = wc < 252 ? bsr(~wc & 0xff) : 1;
    need = 7 - msb;
    wc &= ((1u << msb) - 1) | 0b00000011;
    for (j = 1; j < need; ++j) {
      if ((cb = get(arg, i++)) == -1) return -1;
      if ((cb & 0b11000000) == 0b10000000) {
        wc = wc << 6 | (cb & 0b00111111);
      } else {
        if (out) *out = u'�';
        return -1;
      }
    }
  }
  if (__builtin_expect(!!out, 1)) *out = (wint_t)wc;
  return i;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TPDECODECB_H_ */
