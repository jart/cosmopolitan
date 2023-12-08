/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/itoa.h"
#include "tool/lambda/lib/blc.h"

#define FREEBIES u"ɐqɔpǝɟƃɥıɾʞןɯuodbɹsʇnʌʍxʎz"

/* clang-format off */
#define ALPHABET                \
  u"abcdefghijklmnopqrsuvwxyz"  \
  u"αβγδεζηθιμξπρςστυφχψϑϕ"     \
  u"ℵℶℷℸ"                       \
  u"идџжлђ"                     \
  u"⅄ℏ℘þæß§£¥₿"                 \
  u"𝘢𝘣𝘤𝘥𝘦𝘧𝘨𝘩𝘪𝘫𝘬𝘭𝘮𝘯𝘰𝘱𝘲𝘳𝘴𝘵𝘶𝘷𝘸𝘹𝘺𝘻" \
  u"𝕒𝕓𝕔𝕕𝕖𝕗𝕘𝕙𝕚𝕛𝕜𝕝𝕞𝕟𝕠𝕡𝕢𝕣𝕤𝕥𝕦𝕧𝕨𝕩𝕪𝕫" \
  u"𝗮𝗯𝗰𝗱𝗲𝗳𝗴𝗵𝗶𝗷𝗸𝗹𝗺𝗻𝗼𝗽𝗾𝗿𝘀𝘁𝘂𝘃𝘄𝘅𝘆𝘇"
/* clang-format on */

static char kFalse[] = {
    ABS,     //  0: false
    ABS,     //  1: (λ 0)
    VAR, 0,  //  2: 0
};

static char kTrue[] = {
    ABS,     //  0: true
    ABS,     //  1: (λ 1)
    VAR, 1,  //  2: 1
};

static char kOne[] = {
    ABS,     //  4: (λab.ab)
    ABS,     //  5: (λa.ɐa)
    APP, 2,  //  6: qɐ
    VAR, 1,  //  8: q
    VAR, 0,  // 10: ɐ
};

static char kTwo[] = {
    ABS,     // 12: (λab.a(ab))
    ABS,     // 13: (λa.ɐ(ɐa))
    APP, 2,  // 14: q(qɐ)
    VAR, 1,  // 16: q
    APP, 2,  // 18: qɐ
    VAR, 1,  // 20: q
    VAR, 0,  // 22: ɐ
};

static char kThree[] = {
    ABS,     // 24: (λab.a(a(ab)))
    ABS,     // 25: (λa.ɐ(ɐ(ɐa)))
    APP, 2,  // 26: q(q(qɐ))
    VAR, 1,  // 28: q
    APP, 2,  // 30: q(qɐ)
    VAR, 1,  // 32: q
    APP, 2,  // 34: qɐ
    VAR, 1,  // 36: q
    VAR, 0,  // 38: ɐ
};

static char kFour[] = {
    ABS,     // 40: (λab.a(a(a(ab))))
    ABS,     // 41: (λa.ɐ(ɐ(ɐ(ɐa))))
    APP, 2,  // 42: q(q(q(qɐ)))
    VAR, 1,  // 44: q
    APP, 2,  // 46: q(q(qɐ))
    VAR, 1,  // 48: q
    APP, 2,  // 50: q(qɐ)
    VAR, 1,  // 52: q
    APP, 2,  // 54: qɐ
    VAR, 1,  // 56: q
    VAR, 0,  // 58: ɐ
};

static char kFive[] = {
    ABS,     // 60: (λab.a(a(a(a(ab)))))
    ABS,     // 61: (λa.ɐ(ɐ(ɐ(ɐ(ɐa)))))
    APP, 2,  // 62: q(q(q(q(qɐ))))
    VAR, 1,  // 64: q
    APP, 2,  // 66: q(q(q(qɐ)))
    VAR, 1,  // 68: q
    APP, 2,  // 70: q(q(qɐ))
    VAR, 1,  // 72: q
    APP, 2,  // 74: q(qɐ)
    VAR, 1,  // 76: q
    APP, 2,  // 78: qɐ
    VAR, 1,  // 80: q
    VAR, 0,  // 82: ɐ
};

static char kSix[] = {
    ABS,     // 84: (λab.a(a(a(a(a(ab))))))
    ABS,     // 85: (λa.ɐ(ɐ(ɐ(ɐ(ɐ(ɐa))))))
    APP, 2,  // 86: q(q(q(q(q(qɐ)))))
    VAR, 1,  // 88: q
    APP, 2,  // 90: q(q(q(q(qɐ))))
    VAR, 1,  // 92: q
    APP, 2,  // 94: q(q(q(qɐ)))
    VAR, 1,  // 96: q
    APP, 2,  // 98: q(q(qɐ))
    VAR, 1,  // 100: q
    APP, 2,  // 102: q(qɐ)
    VAR, 1,  // 104: q
    APP, 2,  // 106: qɐ
    VAR, 1,  // 108: q
    VAR, 0,  // 110: ɐ
};

static char kSeven[] = {
    ABS,     // 112: (λab.a(a(a(a(a(a(ab)))))))
    ABS,     // 113: (λa.ɐ(ɐ(ɐ(ɐ(ɐ(ɐ(ɐa)))))))
    APP, 2,  // 114: q(q(q(q(q(q(qɐ))))))
    VAR, 1,  // 116: q
    APP, 2,  // 118: q(q(q(q(q(qɐ)))))
    VAR, 1,  // 120: q
    APP, 2,  // 122: q(q(q(q(qɐ))))
    VAR, 1,  // 124: q
    APP, 2,  // 126: q(q(q(qɐ)))
    VAR, 1,  // 128: q
    APP, 2,  // 130: q(q(qɐ))
    VAR, 1,  // 132: q
    APP, 2,  // 134: q(qɐ)
    VAR, 1,  // 136: q
    APP, 2,  // 138: qɐ
    VAR, 1,  // 140: q
    VAR, 0,  // 142: ɐ
};

static char kEight[] = {
    ABS,     // 144: (λab.a(a(a(a(a(a(a(ab))))))))
    ABS,     // 145: (λa.ɐ(ɐ(ɐ(ɐ(ɐ(ɐ(ɐ(ɐa))))))))
    APP, 2,  // 146: q(q(q(q(q(q(q(qɐ)))))))
    VAR, 1,  // 148: q
    APP, 2,  // 150: q(q(q(q(q(q(qɐ))))))
    VAR, 1,  // 152: q
    APP, 2,  // 154: q(q(q(q(q(qɐ)))))
    VAR, 1,  // 156: q
    APP, 2,  // 158: q(q(q(q(qɐ))))
    VAR, 1,  // 160: q
    APP, 2,  // 162: q(q(q(qɐ)))
    VAR, 1,  // 164: q
    APP, 2,  // 166: q(q(qɐ))
    VAR, 1,  // 168: q
    APP, 2,  // 170: q(qɐ)
    VAR, 1,  // 172: q
    APP, 2,  // 174: qɐ
    VAR, 1,  // 176: q
    VAR, 0,  // 178: ɐ
};

static char kNine[] = {
    ABS,     // 180: (λab.a(a(a(a(a(a(a(a(ab)))))))))
    ABS,     // 181: (λa.ɐ(ɐ(ɐ(ɐ(ɐ(ɐ(ɐ(ɐ(ɐa)))))))))
    APP, 2,  // 182: q(q(q(q(q(q(q(q(qɐ))))))))
    VAR, 1,  // 184: q
    APP, 2,  // 186: q(q(q(q(q(q(q(qɐ)))))))
    VAR, 1,  // 188: q
    APP, 2,  // 190: q(q(q(q(q(q(qɐ))))))
    VAR, 1,  // 192: q
    APP, 2,  // 194: q(q(q(q(q(qɐ)))))
    VAR, 1,  // 196: q
    APP, 2,  // 198: q(q(q(q(qɐ))))
    VAR, 1,  // 200: q
    APP, 2,  // 202: q(q(q(qɐ)))
    VAR, 1,  // 204: q
    APP, 2,  // 206: q(q(qɐ))
    VAR, 1,  // 208: q
    APP, 2,  // 210: q(qɐ)
    VAR, 1,  // 212: q
    APP, 2,  // 214: qɐ
    VAR, 1,  // 216: q
    VAR, 0,  // 218: ɐ
};

static char kSelf[] = {
    ABS,     //  0: λa.aa
    APP, 2,  //  1: ɐɐ
    VAR, 0,  //  3: ɐ
    VAR, 0,  //  5: ɐ
};

static char kOmega[] = {
    APP, 7,  //  0: ω ω
    ABS,     //  2: ω
    APP, 2,  //  3: ɐ ɐ
    VAR, 0,  //  5: ɐ
    VAR, 0,  //  7: ɐ
    ABS,     //  9: ω
    APP, 2,  // 10: ɐ ɐ
    VAR, 0,  // 12: ɐ
    VAR, 0,  // 14: ɐ
};

static char kIf[] = {
    ABS,     //  0: if
    ABS,     //  1: (λλ 2 1 0)
    ABS,     //  2: (λ 2 1 0)
    APP, 6,  //  3: 2 1 0
    APP, 2,  //  5: 2 1
    VAR, 2,  //  7: 2
    VAR, 1,  //  9: 1
    VAR, 0,  // 11: 0
};

static char kOr[] = {
    ABS,     // 32: λab.bba
    ABS,     // 33: λa.aaɐ
    APP, 6,  // 34: ɐɐq
    APP, 2,  // 36: ɐɐ
    VAR, 0,  // 38: ɐ
    VAR, 0,  // 40: ɐ
    VAR, 1,  // 42: q
};

static char kAnd[] = {
    ABS,     // 32: λab.bab
    ABS,     // 33: λa.aɐa
    APP, 6,  // 34: ɐqɐ
    APP, 2,  // 36: ɐq
    VAR, 0,  // 38: ɐ
    VAR, 1,  // 40: q
    VAR, 0,  // 42: ɐ
};

static char kNot[] = {
    ABS,     // 32: λabc.acb
    ABS,     // 33: λab.ɐba
    ABS,     // 34: λa.qaɐ
    APP, 6,  // 35: ɔɐq
    APP, 2,  // 37: ɔɐ
    VAR, 2,  // 39: ɔ
    VAR, 0,  // 41: ɐ
    VAR, 1,  // 43: q
};

static char kPair[] = {
    ABS,     //  0: (λλλ 0 2 1)
    ABS,     //  1: (λλ 0 2 1)
    ABS,     //  2: (λ 0 2 1)
    APP, 6,  //  3: 0 2 1
    APP, 2,  //  5: 0 2
    VAR, 0,  //  7: 0
    VAR, 2,  //  9: 2
    VAR, 1,  // 11: 1
};

static char kFirst[] = {
    ABS,     //  0: (λ 0 false)
    APP, 2,  //  1: 0 false
    VAR, 0,  //  3: 0
    ABS,     //  5: false
    ABS,     //  6: (λ 0)
    VAR, 0,  //  7: 0
};

static char kSecond[] = {
    ABS,     //  0: (λ 0 true)
    APP, 2,  //  1: 0 true
    VAR, 0,  //  3: 0
    ABS,     //  5: true
    ABS,     //  6: (λ 1)
    VAR, 1,  //  7: 1
};

static char kSucc[] = {
    ABS,     //  0: (λλλ 1 (2 1 0))
    ABS,     //  1: (λλ 1 (2 1 0))
    ABS,     //  2: (λ 1 (2 1 0))
    APP, 2,  //  3: 1 (2 1 0)
    VAR, 1,  //  5: 1
    APP, 6,  //  7: 2 1 0
    APP, 2,  //  9: 2 1
    VAR, 2,  // 11: 2
    VAR, 1,  // 13: 1
    VAR, 0,  // 15: 0
};

static char kCompose[] = {
    ABS,     //  0: (λλλ 2 (1 0))
    ABS,     //  1: (λλ 2 (1 0))
    ABS,     //  2: (λ 2 (1 0))
    APP, 2,  //  3: 2 (1 0)
    VAR, 2,  //  5: 2
    APP, 2,  //  7: 1 0
    VAR, 1,  //  9: 1
    VAR, 0,  // 11: 0
};

static char kMap[] = {
    ABS,      //  0: (λλλλ 2 (compose 1 3) 0)
    ABS,      //  1: (λλλ 2 (compose 1 3) 0)
    ABS,      //  2: (λλ 2 (compose 1 3) 0)
    ABS,      //  3: (λ 2 (compose 1 3) 0)
    APP, 25,  //  4: 2 (compose 1 3) 0
    APP, 2,   //  6: 2 (compose 1 3)
    VAR, 2,   //  8: 2
    APP, 17,  // 10: compose 1 3
    APP, 13,  // 12: compose 1
    ABS,      // 14: compose
    ABS,      // 15: (λλ 2 (1 0))
    ABS,      // 16: (λ 2 (1 0))
    APP, 2,   // 17: 2 (1 0)
    VAR, 2,   // 19: 2
    APP, 2,   // 21: 1 0
    VAR, 1,   // 23: 1
    VAR, 0,   // 25: 0
    VAR, 1,   // 27: 1
    VAR, 3,   // 29: 3
    VAR, 0,   // 31: 0
};

static char kCons[] = {
    ABS,     //  0: (λλλλ 1 3 (2 1 0))
    ABS,     //  1: (λλλ 1 3 (2 1 0))
    ABS,     //  2: (λλ 1 3 (2 1 0))
    ABS,     //  3: (λ 1 3 (2 1 0))
    APP, 6,  //  4: 1 3 (2 1 0)
    APP, 2,  //  6: 1 3
    VAR, 1,  //  8: 1
    VAR, 3,  // 10: 3
    APP, 6,  // 12: 2 1 0
    APP, 2,  // 14: 2 1
    VAR, 2,  // 16: 2
    VAR, 1,  // 18: 1
    VAR, 0,  // 20: 0
};

static char kY[] = {
    ABS,     // 32: λa.(λb.bb)(λb.a(bb))
    APP, 7,  // 33: (λa.aa)(λa.ɐ(aa))
    ABS,     // 35: λa.aa
    APP, 2,  // 36: ɐɐ
    VAR, 0,  // 38: ɐ
    VAR, 0,  // 40: ɐ
    ABS,     // 42: λa.ɐ(aa)
    APP, 2,  // 43: q(ɐɐ)
    VAR, 1,  // 45: q
    APP, 2,  // 47: ɐɐ
    VAR, 0,  // 49: ɐ
    VAR, 0,  // 51: ɐ
};

static char kYCurry[] = {
    ABS,      //  0: (λ (λ 1 (0 0)) (λ 1 (0 0)))
    APP, 11,  //  1: (λ 1 (0 0)) (λ 1 (0 0))
    ABS,      //  3: (λ 1 (0 0))
    APP, 2,   //  4: 1 (0 0)
    VAR, 1,   //  6: 1
    APP, 2,   //  8: 0 0
    VAR, 0,   // 10: 0
    VAR, 0,   // 12: 0
    ABS,      // 14: (λ 1 (0 0))
    APP, 2,   // 15: 1 (0 0)
    VAR, 1,   // 17: 1
    APP, 2,   // 19: 0 0
    VAR, 0,   // 21: 0
    VAR, 0,   // 23: 0
};

static char kIszero[] = {
    ABS,     // 32: λabc.a(λd.c)b
    ABS,     // 33: λab.ɐ(λc.b)a
    ABS,     // 34: λa.q(λb.a)ɐ
    APP, 7,  // 35: ɔ(λa.ɐ)q
    APP, 2,  // 37: ɔ(λa.ɐ)
    VAR, 2,  // 39: ɔ
    ABS,     // 41: λa.ɐ
    VAR, 1,  // 42: q
    VAR, 1,  // 44: q
};

static char kPred[] = {
    ABS,      //  0: λabc.a(λde.e(db))(λd.c)(λd.d)
    ABS,      //  1: λab.ɐ(λcd.d(ca))(λc.b)(λc.c)
    ABS,      //  2: λa.q(λbc.c(bɐ))(λb.a)(λb.b)
    APP, 21,  //  3: ɔ(λab.b(aq))(λa.ɐ)(λa.a)
    APP, 16,  //  5: ɔ(λab.b(aq))(λa.ɐ)
    APP, 2,   //  7: ɔ(λab.b(aq))
    VAR, 2,   //  9: ɔ
    ABS,      // 11: λab.b(aq)
    ABS,      // 12: λa.a(ɐɔ)
    APP, 2,   // 13: ɐ(qp)
    VAR, 0,   // 15: ɐ
    APP, 2,   // 17: qp
    VAR, 1,   // 19: q
    VAR, 3,   // 21: p
    ABS,      // 23: λa.ɐ
    VAR, 1,   // 24: q
    ABS,      // 26: λa.a
    VAR, 0,   // 27: ɐ
};

static char kXor[] = {
    ABS,      // 32: λab.a(λcd.bdc)b
    ABS,      // 33: λa.ɐ(λbc.acb)a
    APP, 16,  // 34: q(λab.ɐba)ɐ
    APP, 2,   // 36: q(λab.ɐba)
    VAR, 1,   // 38: q
    ABS,      // 40: λab.ɐba
    ABS,      // 41: λa.qaɐ
    APP, 6,   // 42: ɔɐq
    APP, 2,   // 44: ɔɐ
    VAR, 2,   // 46: ɔ
    VAR, 0,   // 48: ɐ
    VAR, 1,   // 50: q
    VAR, 0,   // 52: ɐ
};

static char kAdd[] = {
    ABS,     // 29: λabcd.ac(bcd)
    ABS,     // 30: λabc.ɐb(abc)
    ABS,     // 31: λab.qa(ɐab)
    ABS,     // 32: λa.ɔɐ(qɐa)
    APP, 6,  // 33: pq(ɔqɐ)
    APP, 2,  // 35: pq
    VAR, 3,  // 37: p
    VAR, 1,  // 39: q
    APP, 6,  // 41: ɔqɐ
    APP, 2,  // 43: ɔq
    VAR, 2,  // 45: ɔ
    VAR, 1,  // 47: q
    VAR, 0,  // 49: ɐ
};

static char kSub[] = {
    ABS,      // 51: λab.b(λcde.c(λfg.g(fd))(λf.e)(λf.f))a
    ABS,      // 52: λa.a(λbcd.b(λef.f(ec))(λe.d)(λe.e))ɐ
    APP, 33,  // 53: ɐ(λabc.a(λde.e(db))(λd.c)(λd.d))q
    APP, 2,   // 55: ɐ(λabc.a(λde.e(db))(λd.c)(λd.d))
    VAR, 0,   // 57: ɐ
    ABS,      // 59: λabc.a(λde.e(db))(λd.c)(λd.d)
    ABS,      // 60: λab.ɐ(λcd.d(ca))(λc.b)(λc.c)
    ABS,      // 61: λa.q(λbc.c(bɐ))(λb.a)(λb.b)
    APP, 21,  // 62: ɔ(λab.b(aq))(λa.ɐ)(λa.a)
    APP, 16,  // 64: ɔ(λab.b(aq))(λa.ɐ)
    APP, 2,   // 66: ɔ(λab.b(aq))
    VAR, 2,   // 68: ɔ
    ABS,      // 70: λab.b(aq)
    ABS,      // 71: λa.a(ɐɔ)
    APP, 2,   // 72: ɐ(qp)
    VAR, 0,   // 74: ɐ
    APP, 2,   // 76: qp
    VAR, 1,   // 78: q
    VAR, 3,   // 80: p
    ABS,      // 82: λa.ɐ
    VAR, 1,   // 83: q
    ABS,      // 85: λa.a
    VAR, 0,   // 86: ɐ
    VAR, 1,   // 88: q
};

static char kLe[] = {
    ABS,      //  0: λab.iszero(- a b)
    ABS,      //  1: λa.iszero(- ɐ a)
    APP, 16,  //  2: iszero(- q ɐ)
    ABS,      //  4: iszero
    APP, 9,   //  5: ɐ (λabc.c) ⊤
    APP, 2,   //  7: ɐ (λabc.c)
    VAR, 0,   //  9: ɐ
    ABS,      // 11: λabc.c
    ABS,      // 12: ⊥
    ABS,      // 13: λa.a
    VAR, 0,   // 14: ɐ
    ABS,      // 16: ⊤
    ABS,      // 17: λa.ɐ
    VAR, 1,   // 18: q
    APP, 43,  // 20: - q ɐ
    APP, 39,  // 22: - q
    ABS,      // 24: -
    ABS,      // 25: λa.a dec ɐ
    APP, 33,  // 26: ɐ dec q
    APP, 2,   // 28: ɐ dec
    VAR, 0,   // 30: ɐ
    ABS,      // 32: dec
    ABS,      // 33: λab.ɐ (λcd.d(c a)) (λc.b) (λc.c)
    ABS,      // 34: λa.q (λbc.c(b ɐ)) (λb.a) (λb.b)
    APP, 21,  // 35: ɔ (λab.b(a q)) (λa.ɐ) (λa.a)
    APP, 16,  // 37: ɔ (λab.b(a q)) (λa.ɐ)
    APP, 2,   // 39: ɔ (λab.b(a q))
    VAR, 2,   // 41: ɔ
    ABS,      // 43: λab.b(a q)
    ABS,      // 44: λa.a(ɐ ɔ)
    APP, 2,   // 45: ɐ(q p)
    VAR, 0,   // 47: ɐ
    APP, 2,   // 49: q p
    VAR, 1,   // 51: q
    VAR, 3,   // 53: p
    ABS,      // 55: λa.ɐ
    VAR, 1,   // 56: q
    ABS,      // 58: λa.a
    VAR, 0,   // 59: ɐ
    VAR, 1,   // 61: q
    VAR, 1,   // 63: q
    VAR, 0,   // 65: ɐ
};

static char kEq[] = {
    ABS,      //  0: λab.∧(≤ a b)(≤ b a)
    ABS,      //  1: λa.∧(≤ ɐ a)(≤ a ɐ)
    APP, 89,  //  2: ∧(≤ q ɐ)(≤ ɐ q)
    APP, 12,  //  4: ∧(≤ q ɐ)
    ABS,      //  6: ∧
    ABS,      //  7: λa.ɐ a ɐ
    APP, 6,   //  8: q ɐ q
    APP, 2,   // 10: q ɐ
    VAR, 1,   // 12: q
    VAR, 0,   // 14: ɐ
    VAR, 1,   // 16: q
    APP, 71,  // 18: ≤ q ɐ
    APP, 67,  // 20: ≤ q
    ABS,      // 22: ≤
    ABS,      // 23: λa.iszero(- ɐ a)
    APP, 16,  // 24: iszero(- q ɐ)
    ABS,      // 26: iszero
    APP, 9,   // 27: ɐ (λabc.c) ⊤
    APP, 2,   // 29: ɐ (λabc.c)
    VAR, 0,   // 31: ɐ
    ABS,      // 33: λabc.c
    ABS,      // 34: ⊥
    ABS,      // 35: λa.a
    VAR, 0,   // 36: ɐ
    ABS,      // 38: ⊤
    ABS,      // 39: λa.ɐ
    VAR, 1,   // 40: q
    APP, 43,  // 42: - q ɐ
    APP, 39,  // 44: - q
    ABS,      // 46: -
    ABS,      // 47: λa.a dec ɐ
    APP, 33,  // 48: ɐ dec q
    APP, 2,   // 50: ɐ dec
    VAR, 0,   // 52: ɐ
    ABS,      // 54: dec
    ABS,      // 55: λab.ɐ (λcd.d(c a)) (λc.b) (λc.c)
    ABS,      // 56: λa.q (λbc.c(b ɐ)) (λb.a) (λb.b)
    APP, 21,  // 57: ɔ (λab.b(a q)) (λa.ɐ) (λa.a)
    APP, 16,  // 59: ɔ (λab.b(a q)) (λa.ɐ)
    APP, 2,   // 61: ɔ (λab.b(a q))
    VAR, 2,   // 63: ɔ
    ABS,      // 65: λab.b(a q)
    ABS,      // 66: λa.a(ɐ ɔ)
    APP, 2,   // 67: ɐ(q p)
    VAR, 0,   // 69: ɐ
    APP, 2,   // 71: q p
    VAR, 1,   // 73: q
    VAR, 3,   // 75: p
    ABS,      // 77: λa.ɐ
    VAR, 1,   // 78: q
    ABS,      // 80: λa.a
    VAR, 0,   // 81: ɐ
    VAR, 1,   // 83: q
    VAR, 1,   // 85: q
    VAR, 0,   // 87: ɐ
    VAR, 1,   // 89: q
    VAR, 0,   // 91: ɐ
    APP, 71,  // 93: ≤ ɐ q
    APP, 67,  // 95: ≤ ɐ
    ABS,      // 97: ≤
    ABS,      // 98: λa.iszero(- ɐ a)
    APP, 16,  // 99: iszero(- q ɐ)
    ABS,      // 101: iszero
    APP, 9,   // 102: ɐ (λabc.c) ⊤
    APP, 2,   // 104: ɐ (λabc.c)
    VAR, 0,   // 106: ɐ
    ABS,      // 108: λabc.c
    ABS,      // 109: ⊥
    ABS,      // 110: λa.a
    VAR, 0,   // 111: ɐ
    ABS,      // 113: ⊤
    ABS,      // 114: λa.ɐ
    VAR, 1,   // 115: q
    APP, 43,  // 117: - q ɐ
    APP, 39,  // 119: - q
    ABS,      // 121: -
    ABS,      // 122: λa.a dec ɐ
    APP, 33,  // 123: ɐ dec q
    APP, 2,   // 125: ɐ dec
    VAR, 0,   // 127: ɐ
    ABS,      // 129: dec
    ABS,      // 130: λab.ɐ (λcd.d(c a)) (λc.b) (λc.c)
    ABS,      // 131: λa.q (λbc.c(b ɐ)) (λb.a) (λb.b)
    APP, 21,  // 132: ɔ (λab.b(a q)) (λa.ɐ) (λa.a)
    APP, 16,  // 134: ɔ (λab.b(a q)) (λa.ɐ)
    APP, 2,   // 136: ɔ (λab.b(a q))
    VAR, 2,   // 138: ɔ
    ABS,      // 140: λab.b(a q)
    ABS,      // 141: λa.a(ɐ ɔ)
    APP, 2,   // 142: ɐ(q p)
    VAR, 0,   // 144: ɐ
    APP, 2,   // 146: q p
    VAR, 1,   // 148: q
    VAR, 3,   // 150: p
    ABS,      // 152: λa.ɐ
    VAR, 1,   // 153: q
    ABS,      // 155: λa.a
    VAR, 0,   // 156: ɐ
    VAR, 1,   // 158: q
    VAR, 1,   // 160: q
    VAR, 0,   // 162: ɐ
    VAR, 0,   // 164: ɐ
    VAR, 1,   // 166: q
};

static int termcmp(const int* p, const char* q, size_t n) {
  int c;
  size_t i;
  for (i = 0; i < n; ++i) {
    if ((c = p[i] - q[i])) {
      return c;
    }
  }
  return 0;
}

void PrintVar(int i, FILE* f) {
  char ibuf[22];
  switch (style) {
    case 0:
      FormatInt64(ibuf, i);
      fputs(ibuf, f);
      break;
    case 1:
      if (0 <= i && i < sizeof(ALPHABET) / sizeof(*ALPHABET) - 1) {
        fputwc(ALPHABET[i], f);
      } else if (i < 0 && ~i < sizeof(FREEBIES) / sizeof(*FREEBIES) - 1) {
        fputwc(FREEBIES[~i], f);
      } else {
        ibuf[0] = '?';
        FormatInt64(ibuf + 1, i);
        fputs(ibuf, f);
      }
      break;
    default:
      do {
        fputc('1', f);
      } while (i-- > 0);
      fputc('0', f);
      break;
  }
}

void PrintDebruijn(int x, int head, int depth, FILE* f) {
  char ibuf[22];
  if (0 <= x && x < TERMS) {
    if (mem[x] == ABS) {
      if (!noname) {
        if (x == 14) {
          fputs("put", f);
          return;
        }
        if (x + sizeof(kTrue) / sizeof(*kTrue) <= end &&
            !termcmp(mem + x, kTrue, sizeof(kTrue))) {
          if (asciiname) {
            fputs("true", f);
          } else {
            fputs("⊤", f);
          }
          return;
        }
        if (x + sizeof(kFalse) / sizeof(*kFalse) <= end &&
            !termcmp(mem + x, kFalse, sizeof(kFalse))) {
          if (asciiname) {
            fputs("false", f);
          } else {
            fputs("⊥", f);
          }
          return;
        }
        if (x + sizeof(kOmega) / sizeof(*kOmega) <= end &&
            !termcmp(mem + x, kOmega, sizeof(kOmega))) {
          if (asciiname) {
            fputs("omega", f);
          } else {
            fputs("Ω", f);
          }
          return;
        }
        if (x + sizeof(kSelf) / sizeof(*kSelf) <= end &&
            !termcmp(mem + x, kSelf, sizeof(kSelf))) {
          if (asciiname) {
            fputs("omega", f);
          } else {
            fputs("ω", f);
          }
          return;
        }
        if (x + sizeof(kY) / sizeof(*kY) <= end &&
            !termcmp(mem + x, kY, sizeof(kY))) {
          fputs("Y", f);
          return;
        }
        if (x + sizeof(kYCurry) / sizeof(*kYCurry) <= end &&
            !termcmp(mem + x, kYCurry, sizeof(kYCurry))) {
          fputs("Y", f);
          return;
        }
        if (x + sizeof(kIf) / sizeof(*kIf) <= end &&
            !termcmp(mem + x, kIf, sizeof(kIf))) {
          fputs("if", f);
          return;
        }
        if (x + sizeof(kPair) / sizeof(*kPair) <= end &&
            !termcmp(mem + x, kPair, sizeof(kPair))) {
          fputs("pair", f);
          return;
        }
        if (x + sizeof(kNot) / sizeof(*kNot) <= end &&
            !termcmp(mem + x, kNot, sizeof(kNot))) {
          if (asciiname) {
            fputs("not", f);
          } else {
            fputwc(L'¬', f);
          }
          return;
        }
        if (x + sizeof(kOr) / sizeof(*kOr) <= end &&
            !termcmp(mem + x, kOr, sizeof(kOr))) {
          if (asciiname) {
            fputs("or", f);
          } else {
            fputwc(L'∨', f);
          }
          return;
        }
        if (x + sizeof(kAnd) / sizeof(*kAnd) <= end &&
            !termcmp(mem + x, kAnd, sizeof(kAnd))) {
          if (asciiname) {
            fputs("and", f);
          } else {
            fputwc(L'∧', f);
          }
          return;
        }
        if (x + sizeof(kXor) / sizeof(*kXor) <= end &&
            !termcmp(mem + x, kXor, sizeof(kXor))) {
          if (asciiname) {
            fputs("xor", f);
          } else {
            fputwc(L'⊻', f);
          }
          return;
        }
        if (x + sizeof(kLe) / sizeof(*kLe) <= end &&
            !termcmp(mem + x, kLe, sizeof(kLe))) {
          if (asciiname) {
            fputs("le", f);
          } else {
            fputwc(L'≤', f);
          }
          return;
        }
        if (x + sizeof(kEq) / sizeof(*kEq) <= end &&
            !termcmp(mem + x, kEq, sizeof(kEq))) {
          fputwc(L'=', f);
          return;
        }
        if (x + sizeof(kAdd) / sizeof(*kAdd) <= end &&
            !termcmp(mem + x, kAdd, sizeof(kAdd))) {
          fputs("+", f);
          return;
        }
        if (x + sizeof(kSub) / sizeof(*kSub) <= end &&
            !termcmp(mem + x, kSub, sizeof(kSub))) {
          fputs("-", f);
          return;
        }
        if (x + sizeof(kCompose) / sizeof(*kCompose) <= end &&
            !termcmp(mem + x, kCompose, sizeof(kCompose))) {
          fputs("∘", f);
          return;
        }
        if (x + sizeof(kSucc) / sizeof(*kSucc) <= end &&
            !termcmp(mem + x, kSucc, sizeof(kSucc))) {
          fputs("inc", f);
          return;
        }
        if (x + sizeof(kPred) / sizeof(*kPred) <= end &&
            !termcmp(mem + x, kPred, sizeof(kPred))) {
          fputs("dec", f);
          return;
        }
        if (x + sizeof(kSecond) / sizeof(*kSecond) <= end &&
            !termcmp(mem + x, kSecond, sizeof(kSecond))) {
          fputs("second", f);
          return;
        }
        if (x + sizeof(kFirst) / sizeof(*kFirst) <= end &&
            !termcmp(mem + x, kFirst, sizeof(kFirst))) {
          fputs("first", f);
          return;
        }
        if (x + sizeof(kMap) / sizeof(*kMap) <= end &&
            !termcmp(mem + x, kMap, sizeof(kMap))) {
          fputs("map", f);
          return;
        }
        if (x + sizeof(kIszero) / sizeof(*kIszero) <= end &&
            !termcmp(mem + x, kIszero, sizeof(kIszero))) {
          fputs("iszero", f);
          return;
        }
        if (x + sizeof(kCons) / sizeof(*kCons) <= end &&
            !termcmp(mem + x, kCons, sizeof(kCons))) {
          fputs("cons", f);
          return;
        }
        if (x + sizeof(kOne) / sizeof(*kOne) <= end &&
            !termcmp(mem + x, kOne, sizeof(kOne))) {
          fputs("one", f);
          return;
        }
        if (x + sizeof(kTwo) / sizeof(*kTwo) <= end &&
            !termcmp(mem + x, kTwo, sizeof(kTwo))) {
          fputs("two", f);
          return;
        }
        if (x + sizeof(kThree) / sizeof(*kThree) <= end &&
            !termcmp(mem + x, kThree, sizeof(kThree))) {
          fputs("three", f);
          return;
        }
        if (x + sizeof(kFour) / sizeof(*kFour) <= end &&
            !termcmp(mem + x, kFour, sizeof(kFour))) {
          fputs("four", f);
          return;
        }
        if (x + sizeof(kFive) / sizeof(*kFive) <= end &&
            !termcmp(mem + x, kFive, sizeof(kFive))) {
          fputs("five", f);
          return;
        }
        if (x + sizeof(kSix) / sizeof(*kSix) <= end &&
            !termcmp(mem + x, kSix, sizeof(kSix))) {
          fputs("six", f);
          return;
        }
        if (x + sizeof(kSeven) / sizeof(*kSeven) <= end &&
            !termcmp(mem + x, kSeven, sizeof(kSeven))) {
          fputs("seven", f);
          return;
        }
        if (x + sizeof(kEight) / sizeof(*kEight) <= end &&
            !termcmp(mem + x, kEight, sizeof(kEight))) {
          fputs("eight", f);
          return;
        }
        if (x + sizeof(kNine) / sizeof(*kNine) <= end &&
            !termcmp(mem + x, kNine, sizeof(kNine))) {
          fputs("nine", f);
          return;
        }
      }
      do {
        ++x;
        if (asciiname) {
          fputc('\\', f);
        } else {
          fputwc(L'λ', f);
        }
        if (!(0 <= x && x < TERMS)) goto Overflow;
      } while (mem[x] == ABS);
      fputc(' ', f);
    }
    if (!(0 <= (x + 1) && (x + 1) < TERMS)) goto Overflow;
    if (mem[x] == APP) {
      fputc('[', f);
      PrintDebruijn(x + 2, 1, depth, f);
      fputc(' ', f);
      PrintDebruijn(x + 2 + mem[x + 1], 0, depth, f);
      fputc(']', f);
    } else if (mem[x] == VAR) {
      if (0 <= x + 1 && x + 1 < TERMS) {
        PrintVar(mem[x + 1], f);
      } else {
        fputc(L'‼', f);
        FormatInt64(ibuf, x);
        fputs(ibuf, f);
      }
    } else if (mem[x] == IOP) {
      if (x < 22) {
        if (mem[x + 1] & 1) {
          fputs("put", f);
        } else if (x & 1) {
          fputs("wr1", f);
        } else {
          fputs("wr0", f);
        }
      } else if (x == end) {
        fputs(asciiname ? "gro" : "⋯", f);
      } else {
        fputc(L'!', f);
        FormatInt64(ibuf, x);
        fputs(ibuf, f);
      }
    } else {
      fputc(L'!', f);
      FormatInt64(ibuf, x);
      fputs(ibuf, f);
    }
    return;
  }
Overflow:
  fputc(L'‼', f);
  FormatInt64(ibuf, x);
  fputs(ibuf, f);
}

void PrintLambda(int x, int head, int depth, int apps, FILE* f) {
  int close = 0;
  char ibuf[22];
  if (0 <= x && x < TERMS) {
    if (mem[x] == ABS) {
      if (!noname) {
        if (x == 14) {
          if (asciiname) {
            fputs("put", f);
          } else {
            fputs("⍆", f);
          }
          return;
        }
        if (x + sizeof(kTrue) / sizeof(*kTrue) <= end &&
            !termcmp(mem + x, kTrue, sizeof(kTrue))) {
          if (asciiname) {
            fputs("true", f);
          } else {
            fputs("⊤", f);
          }
          return;
        }
        if (x + sizeof(kFalse) / sizeof(*kFalse) <= end &&
            !termcmp(mem + x, kFalse, sizeof(kFalse))) {
          if (asciiname) {
            fputs("false", f);
          } else {
            fputs("⊥", f);
          }
          return;
        }
        if (x + sizeof(kY) / sizeof(*kY) <= end &&
            !termcmp(mem + x, kY, sizeof(kY))) {
          fputs("Y", f);
          return;
        }
        if (x + sizeof(kYCurry) / sizeof(*kYCurry) <= end &&
            !termcmp(mem + x, kYCurry, sizeof(kYCurry))) {
          fputs("Y", f);
          return;
        }
        if (x + sizeof(kOmega) / sizeof(*kOmega) <= end &&
            !termcmp(mem + x, kOmega, sizeof(kOmega))) {
          if (asciiname) {
            fputs("OMEGA", f);
          } else {
            fputs("Ω", f);
          }
          return;
        }
        if (x + sizeof(kSelf) / sizeof(*kSelf) <= end &&
            !termcmp(mem + x, kSelf, sizeof(kSelf))) {
          if (asciiname) {
            fputs("omega", f);
          } else {
            fputs("ω", f);
          }
          return;
        }
        if (x + sizeof(kNot) / sizeof(*kNot) <= end &&
            !termcmp(mem + x, kNot, sizeof(kNot))) {
          if (asciiname) {
            fputs("not", f);
          } else {
            fputwc(L'¬', f);
          }
          return;
        }
        if (x + sizeof(kOr) / sizeof(*kOr) <= end &&
            !termcmp(mem + x, kOr, sizeof(kOr))) {
          if (asciiname) {
            fputs("or", f);
          } else {
            fputwc(L'∨', f);
          }
          return;
        }
        if (x + sizeof(kXor) / sizeof(*kXor) <= end &&
            !termcmp(mem + x, kXor, sizeof(kXor))) {
          if (asciiname) {
            fputs("xor", f);
          } else {
            fputwc(L'⊻', f);
          }
          return;
        }
        if (x + sizeof(kLe) / sizeof(*kLe) <= end &&
            !termcmp(mem + x, kLe, sizeof(kLe))) {
          if (asciiname) {
            fputs("le", f);
          } else {
            fputwc(L'≤', f);
          }
          return;
        }
        if (x + sizeof(kEq) / sizeof(*kEq) <= end &&
            !termcmp(mem + x, kEq, sizeof(kEq))) {
          fputwc(L'=', f);
          return;
        }
        if (x + sizeof(kAnd) / sizeof(*kAnd) <= end &&
            !termcmp(mem + x, kAnd, sizeof(kAnd))) {
          if (asciiname) {
            fputs("and", f);
          } else {
            fputwc(L'∧', f);
          }
          return;
        }
        if (x + sizeof(kAdd) / sizeof(*kAdd) <= end &&
            !termcmp(mem + x, kAdd, sizeof(kAdd))) {
          fputs("+", f);
          return;
        }
        if (x + sizeof(kSub) / sizeof(*kSub) <= end &&
            !termcmp(mem + x, kSub, sizeof(kSub))) {
          fputs("-", f);
          return;
        }
        if (x + sizeof(kCompose) / sizeof(*kCompose) <= end &&
            !termcmp(mem + x, kCompose, sizeof(kCompose))) {
          if (asciiname) {
            fputs("compose", f);
          } else {
            fputs("∘", f);
          }
          return;
        }
        if (x + sizeof(kOne) / sizeof(*kOne) <= end &&
            !termcmp(mem + x, kOne, sizeof(kOne))) {
          fputc('1', f);
          return;
        }
        if (x + sizeof(kTwo) / sizeof(*kTwo) <= end &&
            !termcmp(mem + x, kTwo, sizeof(kTwo))) {
          fputc('2', f);
          return;
        }
        if (x + sizeof(kThree) / sizeof(*kThree) <= end &&
            !termcmp(mem + x, kThree, sizeof(kThree))) {
          fputc('3', f);
          return;
        }
        if (x + sizeof(kFour) / sizeof(*kFour) <= end &&
            !termcmp(mem + x, kFour, sizeof(kFour))) {
          fputc('4', f);
          return;
        }
        if (x + sizeof(kFive) / sizeof(*kFive) <= end &&
            !termcmp(mem + x, kFive, sizeof(kFive))) {
          fputc('5', f);
          return;
        }
        if (x + sizeof(kSix) / sizeof(*kSix) <= end &&
            !termcmp(mem + x, kSix, sizeof(kSix))) {
          fputc('6', f);
          return;
        }
        if (x + sizeof(kSeven) / sizeof(*kSeven) <= end &&
            !termcmp(mem + x, kSeven, sizeof(kSeven))) {
          fputc('7', f);
          return;
        }
        if (x + sizeof(kEight) / sizeof(*kEight) <= end &&
            !termcmp(mem + x, kEight, sizeof(kEight))) {
          fputc('8', f);
          return;
        }
        if (x + sizeof(kNine) / sizeof(*kNine) <= end &&
            !termcmp(mem + x, kNine, sizeof(kNine))) {
          fputc('9', f);
          return;
        }
        if (x + sizeof(kIf) / sizeof(*kIf) <= end &&
            !termcmp(mem + x, kIf, sizeof(kIf))) {
          fputs("if", f);
          return;
        }
        if (x + sizeof(kPair) / sizeof(*kPair) <= end &&
            !termcmp(mem + x, kPair, sizeof(kPair))) {
          fputs("pair", f);
          return;
        }
        if (x + sizeof(kSucc) / sizeof(*kSucc) <= end &&
            !termcmp(mem + x, kSucc, sizeof(kSucc))) {
          fputs("inc", f);
          return;
        }
        if (x + sizeof(kPred) / sizeof(*kPred) <= end &&
            !termcmp(mem + x, kPred, sizeof(kPred))) {
          fputs("dec", f);
          return;
        }
        if (x + sizeof(kSecond) / sizeof(*kSecond) <= end &&
            !termcmp(mem + x, kSecond, sizeof(kSecond))) {
          fputs("second", f);
          return;
        }
        if (x + sizeof(kFirst) / sizeof(*kFirst) <= end &&
            !termcmp(mem + x, kFirst, sizeof(kFirst))) {
          fputs("first", f);
          return;
        }
        if (x + sizeof(kMap) / sizeof(*kMap) <= end &&
            !termcmp(mem + x, kMap, sizeof(kMap))) {
          fputs("map", f);
          return;
        }
        if (x + sizeof(kIszero) / sizeof(*kIszero) <= end &&
            !termcmp(mem + x, kIszero, sizeof(kIszero))) {
          fputs("iszero", f);
          return;
        }
        if (x + sizeof(kCons) / sizeof(*kCons) <= end &&
            !termcmp(mem + x, kCons, sizeof(kCons))) {
          fputs("cons", f);
          return;
        }
      }
      if (apps) {
        fputc('(', f);
        close = 1;
      }
      if (asciiname) {
        fputc('\\', f);
      } else {
        fputwc(L'λ', f);
      }
      if (safer) {
        fputwc(ALPHABET[depth++], f);
        fputc('.', f);
        PrintLambda(x + 1, head, depth, apps + 1, f);
        if (close) {
          fputc(')', f);
        }
        return;
      }
      do {
        ++x;
        fputwc(ALPHABET[depth++], f);
        if (!(0 <= x && x < TERMS)) goto Overflow;
      } while (mem[x] == ABS);
      fputc('.', f);
    }
    if (!(0 <= (x + 1) && (x + 1) < TERMS)) goto Overflow;
    if (mem[x] == VAR) {
      if (0 <= x + 1 && x + 1 < TERMS) {
        PrintVar(depth - 1 - mem[x + 1], f);
      } else {
        fputc(L'‼', f);
        FormatInt64(ibuf, x);
        fputs(ibuf, f);
      }
    } else if (mem[x] == APP) {
      if (!close && !head) {
        fputc('(', f);
        close = 1;
      }
      PrintLambda(x + 2, 1, depth, apps + 1, f);
      if (!(x + 2 + mem[x + 1] < TERMS && mem[x + 2 + mem[x + 1]] == APP)) {
        if (safer || !noname) fputc(' ', f);
      }
      PrintLambda(x + 2 + mem[x + 1], 0, depth, apps + 1, f);
    } else if (mem[x] == IOP) {
      if (x < 22) {
        if (mem[x + 1] & 1) {
          fputs(asciiname ? "put" : "⍆", f);
        } else if (x & 1) {
          fputs(asciiname ? "wr1" : "⍆₁", f);
        } else {
          fputs(asciiname ? "wr0" : "⍆₀", f);
        }
      } else if (x == end) {
        fputs(asciiname ? "gro" : "⋯", f);
      } else {
        fputc(L'!', f);
        FormatInt64(ibuf, x);
        fputs(ibuf, f);
      }
    } else {
      fputc(L'!', f);
      FormatInt64(ibuf, x);
      fputs(ibuf, f);
    }
    if (close) {
      fputc(')', f);
    }
    return;
  }
Overflow:
  fputc(L'‼', f);
  FormatInt64(ibuf, x);
  fputs(ibuf, f);
}

void PrintBinary(int x, int head, int depth, FILE* f) {
  char ibuf[22];
  if (0 <= x && x < TERMS) {
    if (mem[x] == ABS) {
      if (x == 14) {
        fputs("⍆", f);
        return;
      }
      do {
        ++x;
        ++depth;
        fputc('0', f);
        fputc('0', f);
        if (!(0 <= x && x < TERMS)) goto Overflow;
      } while (mem[x] == ABS);
    }
    if (!(0 <= (x + 1) && (x + 1) < TERMS)) goto Overflow;
    if (mem[x] == VAR) {
      if (0 <= x + 1 && x + 1 < TERMS) {
        PrintVar(mem[x + 1], f);
      } else {
        fputc(L'‼', f);
        FormatInt64(ibuf, x);
        fputs(ibuf, f);
      }
    } else if (mem[x] == APP) {
      fputc('0', f);
      fputc('1', f);
      PrintBinary(x + 2, 0, 0, f);
      PrintBinary(x + 2 + mem[x + 1], 0, 0, f);
    } else if (mem[x] == IOP) {
      if (x < 22) {
        if (mem[x + 1] & 1) {
          fputs("⍆", f);
        } else if (x & 1) {
          fputs("⍆₁", f);
        } else {
          fputs("⍆₀", f);
        }
      } else {
        fputwc(L'⋯', f);
      }
    } else if (mem[x] == -1) {
      fputwc(L'⋯', f);
    } else {
      fputc(L'!', f);
      FormatInt64(ibuf, x);
      fputs(ibuf, f);
    }
    return;
  }
Overflow:
  fputc(L'‼', f);
  FormatInt64(ibuf, x);
  fputs(ibuf, f);
}

void Print(int x, int head, int depth, FILE* f) {
  switch (style) {
    case 0:
      PrintDebruijn(x, head, depth, f);
      break;
    case 1:
      PrintLambda(x, head, depth, 0, f);
      break;
    default:
      PrintBinary(x, head, depth, f);
      break;
  }
}
