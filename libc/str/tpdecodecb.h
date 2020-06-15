/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#ifndef COSMOPOLITAN_LIBC_STR_TPDECODECB_H_
#define COSMOPOLITAN_LIBC_STR_TPDECODECB_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Generic Thompson-Pike Varint Decoder.
 * @return number of bytes successfully consumed or -1 w/ errno
 * @note synchronization is performed
 */
forceinline int tpdecodecb(wint_t *out, int first,
                           int get(void *arg, uint32_t i), void *arg) {
  uint32_t wc, cb, need, msb, i = 1;
  if ((wc = first) == -1) return -1;
  while ((wc & 0b11000000) == 0b10000000) {
    if ((wc = get(arg, i++)) == -1) return -1;
  }
  if ((wc & 0b10000000) == 0b10000000) {
    asm("bsr\t%1,%0" : "=r"(msb) : "rm"(~wc & 0xff) : "cc");
    if (!msb) msb = 1;
    need = 7 - msb;
    wc &= ((1u << msb) - 1) | 0b00000011;
    for (uint32_t j = 1; j < need; ++j) {
      if ((cb = get(arg, i++)) == -1) return -1;
      if ((cb & 0b11000000) == 0b10000000) {
        wc = wc << 6 | (cb & 0b00111111);
      } else {
        if (out) *out = u'�';
        return -1;
      }
    }
  }
  if (out) *out = (wint_t)wc;
  return i;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TPDECODECB_H_ */
