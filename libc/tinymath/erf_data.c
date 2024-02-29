/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Optimized Routines                                                          │
│  Copyright (c) 2018-2024, Arm Limited.                                       │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/tinymath/arm.internal.h"
__static_yoink("arm_optimized_routines_notice");

/*
Minimax approximation of erf
*/
const struct erf_data __erf_data = {
.erf_poly_A = {
#if ERF_POLY_A_NCOEFFS == 10
0x1.06eba8214db68p-3, -0x1.812746b037948p-2, 0x1.ce2f21a03872p-4,
-0x1.b82ce30e6548p-6, 0x1.565bcc360a2f2p-8, -0x1.c02d812bc979ap-11,
0x1.f99bddfc1ebe9p-14, -0x1.f42c457cee912p-17, 0x1.b0e414ec20ee9p-20,
-0x1.18c47fd143c5ep-23
#endif
},
/* Rational approximation on [0x1p-28, 0.84375] */
.erf_ratio_N_A = {
0x1.06eba8214db68p-3, -0x1.4cd7d691cb913p-2, -0x1.d2a51dbd7194fp-6,
-0x1.7a291236668e4p-8, -0x1.8ead6120016acp-16
},
.erf_ratio_D_A = {
0x1.97779cddadc09p-2, 0x1.0a54c5536cebap-4, 0x1.4d022c4d36b0fp-8,
0x1.15dc9221c1a1p-13, -0x1.09c4342a2612p-18
},
/* Rational approximation on [0.84375, 1.25] */
.erf_ratio_N_B = {
-0x1.359b8bef77538p-9, 0x1.a8d00ad92b34dp-2, -0x1.7d240fbb8c3f1p-2,
0x1.45fca805120e4p-2, -0x1.c63983d3e28ecp-4, 0x1.22a36599795ebp-5,
-0x1.1bf380a96073fp-9
},
.erf_ratio_D_B = {
0x1.b3e6618eee323p-4, 0x1.14af092eb6f33p-1, 0x1.2635cd99fe9a7p-4,
0x1.02660e763351fp-3, 0x1.bedc26b51dd1cp-7, 0x1.88b545735151dp-7
},
.erfc_poly_C = {
#if ERFC_POLY_C_NCOEFFS == 16
/* Generated using Sollya::remez(f(c*x+d), deg, [(a-d)/c;(b-d)/c], 1, 1e-16), [|D ...|] with deg=15 a=1.25 b=2 c=1 d=1.25 */
0x1.3bcd133aa0ffcp-4, -0x1.e4652fadcb702p-3, 0x1.2ebf3dcca0446p-2,
-0x1.571d01c62d66p-3, 0x1.93a9a8f5b3413p-8, 0x1.8281cbcc2cd52p-5,
-0x1.5cffd86b4de16p-6, -0x1.db4ccf595053ep-9, 0x1.757cbf8684edap-8,
-0x1.ce7dfd2a9e56ap-11, -0x1.99ee3bc5a3263p-11, 0x1.3c57cf9213f5fp-12,
0x1.60692996bf254p-14, -0x1.6e44cb7c1fa2ap-14, 0x1.9d4484ac482b2p-16,
-0x1.578c9e375d37p-19
#endif
},
.erfc_poly_D = {
#if ERFC_POLY_D_NCOEFFS == 18
/* Generated using Sollya::remez(f(c*x+d), deg, [(a-d)/c;(b-d)/c], 1, 1e-16), [|D ...|] with deg=17 a=2 b=3.25 c=2 d=2 */
0x1.328f5ec350e5p-8, -0x1.529b9e8cf8e99p-5, 0x1.529b9e8cd9e71p-3,
-0x1.8b0ae3a023bf2p-2, 0x1.1a2c592599d82p-1, -0x1.ace732477e494p-2,
-0x1.e1a06a27920ffp-6, 0x1.bae92a6d27af6p-2, -0x1.a15470fcf5ce7p-2,
0x1.bafe45d18e213p-6, 0x1.0d950680d199ap-2, -0x1.8c9481e8f22e3p-3,
-0x1.158450ed5c899p-4, 0x1.c01f2973b44p-3, -0x1.73ed2827546a7p-3,
0x1.47733687d1ff7p-4, -0x1.2dec70d00b8e1p-6, 0x1.a947ab83cd4fp-10
#endif
},
.erfc_poly_E = {
#if ERFC_POLY_E_NCOEFFS == 14
/* Generated using Sollya::remez(f(c*x+d), deg, [(a-d)/c;(b-d)/c], 1, 1e-16), [|D ...|] with deg=13 a=3.25 b=4 c=1 d=3.25 */
0x1.20c13035539e4p-18, -0x1.e9b5e8d16df7ep-16, 0x1.8de3cd4733bf9p-14,
-0x1.9aa48beb8382fp-13, 0x1.2c7d713370a9fp-12, -0x1.490b12110b9e2p-12,
0x1.1459c5d989d23p-12, -0x1.64b28e9f1269p-13, 0x1.57c76d9d05cf8p-14,
-0x1.bf271d9951cf8p-16, 0x1.db7ea4d4535c9p-19, 0x1.91c2e102d5e49p-20,
-0x1.e9f0826c2149ep-21, 0x1.60eebaea236e1p-23
#endif
},
.erfc_poly_F = {
#if ERFC_POLY_F_NCOEFFS == 17
/* Generated using Sollya::remez(f(c*x+d), deg, [(a-d)/c;(b-d)/c], 1, 1e-16), [|D ...|] with deg=16 a=4 b=5.90625 c=2 d=4 */
0x1.08ddd130d1fa6p-26, -0x1.10b146f59ff06p-22, 0x1.10b135328b7b2p-19,
-0x1.6039988e7575fp-17, 0x1.497d365e19367p-15, -0x1.da48d9afac83ep-14,
0x1.1024c9b1fbb48p-12, -0x1.fc962e7066272p-12, 0x1.87297282d4651p-11,
-0x1.f057b255f8c59p-11, 0x1.0228d0eee063p-10, -0x1.b1b21b84ec41cp-11,
0x1.1ead8ae9e1253p-11, -0x1.1e708fba37fccp-12, 0x1.9559363991edap-14,
-0x1.68c827b783d9cp-16, 0x1.2ec4adeccf4a2p-19
#endif
}
};
