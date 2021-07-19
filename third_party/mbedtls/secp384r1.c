/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/log/check.h"
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/ecp_internal.h"
#include "third_party/mbedtls/math.h"

#define Q(i) p[i >> 1]

/**
 * Fastest quasi-reduction modulo Prime 384.
 *
 *     p  = 2³⁸⁴ – 2¹²⁸ – 2⁶ + 2³² – 1
 *     B  = T + 2×S₁ + S₂ + S₃ + S₄ + S₅ + S₆ – D₁ – D₂ – D₃ mod p
 *     T  = (A₁₁‖A₁₀‖A₉ ‖A₈ ‖A₇ ‖A₆ ‖A₅ ‖A₄ ‖A₃ ‖A₂ ‖A₁ ‖A₀ )
 *     S₁ = (0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖0  ‖0  ‖0  ‖0  )
 *     S₂ = (A₂₃‖A₂₂‖A₂₁‖A₂₀‖A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂)
 *     S₃ = (A₂₀‖A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂‖A₂₃‖A₂₂‖A₂₁)
 *     S₄ = (A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂‖A₂₀‖0  ‖A₂₃‖0  )
 *     S₅ = (0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖A₂₀‖0  ‖0  ‖0  ‖0  )
 *     S₆ = (0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖0  ‖0  ‖A₂₀)
 *     D₁ = (A₂₂‖A₂₁‖A₂₀‖A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂‖A₂₃)
 *     D₂ = (0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖A₂₀‖0  )
 *     D₃ = (0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₃‖0  ‖0  ‖0  )
 *
 * @see FIPS 186-3 §D.2.4
 */
void secp384r1(uint64_t p[12]) {
  int r;
  char o;
  signed char G;
  uint64_t A, B, C, D, E, F, a, b, c;
  A = Q(0);
  B = Q(2);
  C = Q(4);
  D = Q(6);
  E = Q(8);
  F = Q(10);
  G = 0;
#if !defined(__x86_64__) || defined(__STRICT_ANSI__)
  a = Q(22) << 32 | Q(21) >> 32;
  b = Q(23) >> 32;
  ADC(C, C, a << 1, 0, o);
  ADC(D, D, (b << 1 | a >> 63), o, o);
  ADC(E, E, (b >> 63), o, o);
  ADC(F, F, o, o, o);
  G += o;
  ADC(A, A, Q(12), 0, o);
  ADC(B, B, Q(14), o, o);
  ADC(C, C, Q(16), o, o);
  ADC(D, D, Q(18), o, o);
  ADC(E, E, Q(20), o, o);
  ADC(F, F, Q(22), o, o);
  G += o;
  ADC(A, A, Q(22) << 32 | Q(21) >> 32, 0, o);
  ADC(B, B, Q(12) << 32 | Q(23) >> 32, o, o);
  ADC(C, C, Q(14) << 32 | Q(13) >> 32, o, o);
  ADC(D, D, Q(16) << 32 | Q(15) >> 32, o, o);
  ADC(E, E, Q(18) << 32 | Q(17) >> 32, o, o);
  ADC(F, F, Q(20) << 32 | Q(19) >> 32, o, o);
  G += o;
  ADC(A, A, Q(23) >> 32 << 32, 0, o);
  ADC(B, B, Q(20) << 32, o, o);
  ADC(C, C, Q(12), o, o);
  ADC(D, D, Q(14), o, o);
  ADC(E, E, Q(16), o, o);
  ADC(F, F, Q(18), o, o);
  G += o;
  ADC(C, C, Q(20), 0, o);
  ADC(D, D, Q(22), o, o);
  ADC(E, E, 0, o, o);
  ADC(F, F, 0, o, o);
  G += o;
  ADC(A, A, Q(20) & 0xffffffff, 0, o);
  ADC(B, B, Q(21) >> 32 << 32, o, o);
  ADC(C, C, Q(22), o, o);
  ADC(D, D, 0, o, o);
  ADC(E, E, 0, o, o);
  ADC(F, F, 0, o, o);
  G += o;
  SBB(A, A, Q(12) << 32 | Q(23) >> 32, 0, o);
  SBB(B, B, Q(14) << 32 | Q(13) >> 32, o, o);
  SBB(C, C, Q(16) << 32 | Q(15) >> 32, o, o);
  SBB(D, D, Q(18) << 32 | Q(17) >> 32, o, o);
  SBB(E, E, Q(20) << 32 | Q(19) >> 32, o, o);
  SBB(F, F, Q(22) << 32 | Q(21) >> 32, o, o);
  G -= o;
  SBB(A, A, Q(20) << 32, 0, o);
  SBB(B, B, Q(22) << 32 | Q(21) >> 32, o, o);
  SBB(C, C, Q(23) >> 32, o, o);
  SBB(D, D, 0, o, o);
  SBB(E, E, 0, o, o);
  SBB(F, F, 0, o, o);
  G -= o;
  SBB(B, B, Q(23) >> 32 << 32, 0, o);
  SBB(C, C, Q(23) >> 32, o, o);
  SBB(D, D, 0, o, o);
  SBB(E, E, 0, o, o);
  SBB(F, F, 0, o, o);
  G -= o;
#else
  asm volatile(/* S₁ = (0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖0  ‖0  ‖0  ‖0  ) */
               "mov\t21*4(%9),%7\n\t"
               "mov\t23*4(%9),%k8\n\t"
               "shl\t%7\n\t"
               "rcl\t%8\n\t"
               "add\t%7,%2\n\t"
               "adc\t%8,%3\n\t"
               "adc\t$0,%4\n\t"
               "adc\t$0,%5\n\t"
               "adc\t$0,%b6\n\t"
               /* S₂ = (A₂₃‖A₂₂‖A₂₁‖A₂₀‖A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂) */
               "add\t12*4(%9),%0\n\t"
               "adc\t14*4(%9),%1\n\t"
               "adc\t16*4(%9),%2\n\t"
               "adc\t18*4(%9),%3\n\t"
               "adc\t20*4(%9),%4\n\t"
               "adc\t22*4(%9),%5\n\t"
               "adc\t$0,%b6\n\t"
               /* S₃ = (A₂₀‖A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂‖A₂₃‖A₂₂‖A₂₁) */
               "mov\t12*4(%9),%k7\n\t"
               "mov\t23*4(%9),%k8\n\t"
               "shl\t$32,%7\n\t"
               "or\t%7,%8\n\t"
               "add\t21*4(%9),%0\n\t"
               "adc\t%8,%1\n\t"
               "adc\t13*4(%9),%2\n\t"
               "adc\t15*4(%9),%3\n\t"
               "adc\t17*4(%9),%4\n\t"
               "adc\t19*4(%9),%5\n\t"
               "adc\t$0,%b6\n\t"
               /* S₄ = (A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂‖A₂₀‖0  ‖A₂₃‖0  ) */
               "mov\t23*4(%9),%k7\n\t"
               "mov\t20*4(%9),%k8\n\t"
               "shl\t$32,%7\n\t"
               "shl\t$32,%8\n\t"
               "add\t%7,%0\n\t"
               "adc\t%8,%1\n\t"
               "adc\t12*4(%9),%2\n\t"
               "adc\t14*4(%9),%3\n\t"
               "adc\t16*4(%9),%4\n\t"
               "adc\t18*4(%9),%5\n\t"
               "adc\t$0,%b6\n\t"
               /* S₅ = (0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖A₂₀‖0  ‖0  ‖0  ‖0  ) */
               "mov\t23*4(%9),%k7\n\t"
               "mov\t20*4(%9),%k8\n\t"
               "shl\t$32,%7\n\t"
               "shl\t$32,%8\n\t"
               "add\t20*4(%9),%2\n\t"
               "adc\t22*4(%9),%3\n\t"
               "adc\t$0,%4\n\t"
               "adc\t$0,%5\n\t"
               "adc\t$0,%b6\n\t"
               /* S₆ = (0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖0  ‖0  ‖A₂₀) */
               "mov\t20*4(%9),%k7\n\t"
               "mov\t21*4(%9),%k8\n\t"
               "shl\t$32,%8\n\t"
               "add\t%7,%0\n\t"
               "adc\t%8,%1\n\t"
               "adc\t22*4(%9),%2\n\t"
               "adc\t$0,%3\n\t"
               "adc\t$0,%4\n\t"
               "adc\t$0,%5\n\t"
               "adc\t$0,%b6\n\t"
               /* D₁ = (A₂₂‖A₂₁‖A₂₀‖A₁₉‖A₁₈‖A₁₇‖A₁₆‖A₁₅‖A₁₄‖A₁₃‖A₁₂‖A₂₃) */
               "mov\t23*4(%9),%k7\n\t"
               "mov\t12*4(%9),%k8\n\t"
               "shl\t$32,%8\n\t"
               "or\t%8,%7\n\t"
               "sub\t%7,%0\n\t"
               "sbb\t13*4(%9),%1\n\t"
               "sbb\t15*4(%9),%2\n\t"
               "sbb\t17*4(%9),%3\n\t"
               "sbb\t19*4(%9),%4\n\t"
               "sbb\t21*4(%9),%5\n\t"
               "sbb\t$0,%b6\n\t"
               /* D₂ = (0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₂‖A₂₁‖A₂₀‖0  ) */
               "mov\t20*4(%9),%k7\n\t"
               "mov\t23*4(%9),%k8\n\t"
               "shl\t$32,%7\n\t"
               "sub\t%7,%0\n\t"
               "sbb\t21*4(%9),%1\n\t"
               "sbb\t%8,%2\n\t"
               "sbb\t$0,%3\n\t"
               "sbb\t$0,%4\n\t"
               "sbb\t$0,%5\n\t"
               "sbb\t$0,%b6\n\t"
               /* D₃ = (0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖0  ‖A₂₃‖A₂₃‖0  ‖0  ‖0  ) */
               "mov\t23*4(%9),%k7\n\t"
               "mov\t%k7,%k8\n\t"
               "shl\t$32,%7\n\t"
               "sub\t%7,%1\n\t"
               "sbb\t%8,%2\n\t"
               "sbb\t$0,%3\n\t"
               "sbb\t$0,%4\n\t"
               "sbb\t$0,%5\n\t"
               "sbb\t$0,%b6\n\t"
               : "+r"(A), "+r"(B), "+r"(C), "+r"(D), "+r"(E), "+r"(F), "+q"(G),
                 "=&r"(a), "=&r"(b)
               : "r"(p)
               : "memory");
#endif
  p[0] = A;
  p[1] = B;
  p[2] = C;
  p[3] = D;
  p[4] = E;
  p[5] = F;
  p[6] = G;
  p[7] = 0;
  p[8] = 0;
  p[9] = 0;
  p[10] = 0;
  p[11] = 0;
}

int ecp_mod_p384(mbedtls_mpi *N) {
  int r;
  char o;
  if (N->n < 12 && (r = mbedtls_mpi_grow(N, 12))) return r;
  secp384r1(N->p);
  if ((int64_t)N->p[6] < 0) {
    N->s = -1;
    SBB(N->p[0], 0, N->p[0], 0, o);
    SBB(N->p[1], 0, N->p[1], o, o);
    SBB(N->p[2], 0, N->p[2], o, o);
    SBB(N->p[3], 0, N->p[3], o, o);
    SBB(N->p[4], 0, N->p[4], o, o);
    SBB(N->p[5], 0, N->p[5], o, o);
    N->p[6] = 0 - (N->p[6] + o);
  } else {
    N->s = 1;
  }
  return 0;
}
