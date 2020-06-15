/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#ifndef COSMOPOLITAN_LIBC_STR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STR_INTERNAL_H_
#include "libc/str/str.h"

#if !(__ASSEMBLER__ + __LINKER__ + 0)

hidden extern const uint32_t kSha256Tab[64];

extern const struct TpEncode {
  uint8_t mark;
  uint8_t len;
} kTpDecoderRing[32];

forceinline struct TpEncode UseTpDecoderRing(wint_t c) {
  unsigned msb;
  if (c) {
    asm("bsr\t%1,%0" : "=r"(msb) : "rm"(c) : "cc");
  } else {
    msb = 0;
  }
  return kTpDecoderRing[msb];
}

nodebuginfo forceinline bool32 ismoar(wint_t c) {
  return (c & 0b11000000) == 0b11000000;
}

nodebuginfo forceinline bool32 iscont(wint_t c) {
  return (c & 0b11000000) == 0b10000000;
}

extern const uint32_t kCrc32Tab[256];

char *strstr$sse42(const char *, const char *) strlenesque hidden;
char16_t *strstr16$sse42(const char16_t *, const char16_t *) strlenesque hidden;
void *memmem$sse42(const void *, size_t, const void *,
                   size_t) strlenesque hidden;
uint32_t crc32c$sse42(uint32_t, const void *, size_t) strlenesque hidden;
uint32_t crc32$pclmul(uint32_t, const void *, size_t) hidden;
void sha256$x86(uint32_t[hasatleast 8], const uint8_t[hasatleast 64],
                uint32_t) hidden;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_INTERNAL_H_ */
