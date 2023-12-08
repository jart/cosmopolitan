/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/errfuns.h"

static inline void add_set(unsigned __int128 *pResTemp, uint16_t *pCurrentSet,
                           unsigned *pDigitsLeft) {
  *pDigitsLeft -= *pDigitsLeft % 4;
  *pResTemp = (*pResTemp << 16) | *pCurrentSet;
  *pCurrentSet = 0;
}

static int inet_pton_inet6_impl(const char *src, uint8_t *dst) {
  enum STATES {
    COLON_OK = 1 << 0,
    COLON_WAS = 1 << 1,
    DIGIT_OK = 1 << 2,
    COLON_OR_DIGIT = COLON_OK | DIGIT_OK,
  };

  unsigned __int128 res = 0;
  unsigned __int128 resTemp = 0;
  char c;
  enum STATES state = COLON_OR_DIGIT;
  unsigned digitsLeft = 32;
  bool zeroFound = false;
  uint16_t currentSet = 0;
  while ((c = *src++)) {
    if (digitsLeft == 0) {
      return 0;
    }

    if (state & COLON_OK) {
      if (c == ':') {
        if (state & COLON_WAS) {
          if (zeroFound) {
            return 0;
          }
          if (digitsLeft == 32) {
            res = 0;
          } else {
            res = resTemp << (4 * digitsLeft);
          }
          resTemp = 0;
          digitsLeft -= 4;
          zeroFound = true;
          state = DIGIT_OK;
        } else {
          if (digitsLeft % 4) {
            add_set(&resTemp, &currentSet, &digitsLeft);
          } else if (digitsLeft == 32) {
            state = COLON_OK;
          } else {
            state = COLON_OR_DIGIT;
          }
          state |= COLON_WAS;
        }
        continue;
      }
    }

    if (state & DIGIT_OK) {
      if ((c >= '0') && ((c < ':') || ((c >= 'A') && (c < 'G')) ||
                         ((c >= 'a') && (c < 'g')))) {
        state &= (~COLON_WAS);
        uint8_t digit;
        if (c < ':') {
          digit = c - '0';
        } else if (c < 'G') {
          digit = c - 'A' + 10;
        } else {
          digit = c - 'a' + 10;
        }
        currentSet = (currentSet << 4) | digit;
        digitsLeft--;
        if (!(digitsLeft % 4)) {
          state = COLON_OK;
          add_set(&resTemp, &currentSet, &digitsLeft);
        } else if ((state == DIGIT_OK) && (digitsLeft > 4)) {
          state = COLON_OR_DIGIT;
        }
        continue;
      } else if ((c == '.') && (digitsLeft >= 5) && (digitsLeft <= 27) &&
                 (digitsLeft % 4)) {
        uint8_t ipv4[4];
        const unsigned digitsRead = 4 - digitsLeft % 4;
        if (inet_pton(AF_INET, src - (1 + digitsRead), &ipv4) == 1) {
          state &= (~COLON_WAS);
          digitsLeft += digitsRead;
          currentSet = (ipv4[0] << 8) | ipv4[1];
          digitsLeft -= 4;
          add_set(&resTemp, &currentSet, &digitsLeft);
          currentSet = (ipv4[2] << 8) | ipv4[3];
          digitsLeft -= 4;
          add_set(&resTemp, &currentSet, &digitsLeft);
          break;
        }
      }
    }
    return 0;
  }
  if (state & COLON_WAS) {
    return 0;
  } else if (digitsLeft % 4) {
    add_set(&resTemp, &currentSet, &digitsLeft);
  }
  if (!zeroFound && (digitsLeft != 0)) {
    return 0;
  }

  res |= resTemp;
  dst[0] = res >> (15 * 8);
  dst[1] = res >> (14 * 8);
  dst[2] = res >> (13 * 8);
  dst[3] = res >> (12 * 8);
  dst[4] = res >> (11 * 8);
  dst[5] = res >> (10 * 8);
  dst[6] = res >> (9 * 8);
  dst[7] = res >> (8 * 8);
  dst[8] = res >> (7 * 8);
  dst[9] = res >> (6 * 8);
  dst[10] = res >> (5 * 8);
  dst[11] = res >> (4 * 8);
  dst[12] = res >> (3 * 8);
  dst[13] = res >> (2 * 8);
  dst[14] = res >> (1 * 8);
  dst[15] = res >> (0 * 8);
  return 1;
}

/**
 * Converts internet address string to binary.
 *
 * @param af can be AF_INET or AF_INET6
 * @param src is the ASCII-encoded address
 * @param dst is where the binary-encoded net-order address goes
 * @return 1 on success, 0 on src malformed, or -1 w/ errno
 */
int inet_pton(int af, const char *src, void *dst) {
  uint8_t *p;
  int b, c, j;
  if (af == AF_INET6) return inet_pton_inet6_impl(src, dst);
  if (af != AF_INET) return eafnosupport();
  j = 0;
  p = dst;
  p[0] = 0;
  while ((c = *src++)) {
    if (isdigit(c)) {
      b = c - '0' + p[j] * 10;
      p[j] = MIN(255, b);
      if (b > 255) return 0;
    } else if (c == '.') {
      if (++j == 4) return 0;
      p[j] = 0;
    } else {
      return 0;
    }
  }
  return j == 3 ? 1 : 0;
}
