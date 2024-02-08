/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#ifndef APE_MACROS_H_
#define APE_MACROS_H_
#include "libc/macros.internal.h"
#ifdef __ASSEMBLER__
/* clang-format off */

//	Initializes real mode stack.
//	The most holiest of holy code.
//	@mode	real
//	@see	www.pcjs.org/pubs/pc/reference/intel/8086/
.macro	rlstack	seg:req addr:req
	cli
	mov	\seg,%ss
	mov	\addr,%sp
	sti
.endm

//	Symbolic Linker-Defined Binary Content.
.macro	.stub	name:req kind:req default type=@object
 .ifnb	\default
  .equ	\name,\default
 .endif
 .\kind	\name
 .type	\name,\type
 .weak	\name
.hidden	\name
.endm

//	Symbolic Linker-Defined Binary-Encoded-Bourne Content.
//	@param	units is the number of encoded 32-bit values to insert,
//		e.g. \000 can be encoded as 0x3030305c.
.macro	.shstub	name:req num:req
 ss	\name,0
 .if	\num>1
  ss	\name,1
   .if	\num>2
    ss	\name,2
    ss	\name,3
    .if	\num>4
     ss	\name,4
     ss	\name,5
     ss	\name,6
     ss	\name,7
   .endif
  .endif
 .endif
.endm
.macro	ss	name n
 .stub	\name\()_bcs\n,long
.endm

//	Task State Segment Descriptor Entries.
.macro	.tssdescstub name:req
 .ifndef \name
  .weak	\name
  .set	\name,0
 .endif
 .ifndef \name\()_end
  .weak	\name\()_end
  .set	\name\()_end,0
 .endif
 .stub	\name\()_desc_ent0,quad
 .stub	\name\()_desc_ent1,quad
.endm

/* clang-format on */
#elif defined(__LINKER__)

#define BCX_NIBBLE(X) ((((X)&0xf) > 0x9) ? ((X)&0xf) + 0x37 : ((X)&0xf) + 0x30)
#define BCX_OCTET(X)  ((BCX_NIBBLE((X) >> 4) << 8) | (BCX_NIBBLE((X) >> 0) << 0))
#define BCX_INT16(X)  ((BCX_OCTET((X) >> 8) << 16) | (BCX_OCTET((X) >> 0) << 0))
#define BCXSTUB(SYM, X)                      \
  HIDDEN(SYM##_bcx0 = BCX_INT16((X) >> 48)); \
  HIDDEN(SYM##_bcx1 = BCX_INT16((X) >> 32)); \
  HIDDEN(SYM##_bcx2 = BCX_INT16((X) >> 16)); \
  HIDDEN(SYM##_bcx3 = BCX_INT16((X) >> 0))

/**
 * Binary coded backslash octet support.
 *
 * <p>This allows linker scripts to generate printf commands.
 */
#define BCO_OCTET(X) (((X)&0x7) + 0x30)
#define BCOB_UNIT(X)                                           \
  ((BCO_OCTET((X) >> 0) << 24) | (BCO_OCTET((X) >> 3) << 16) | \
   (BCO_OCTET(((X)&0xff) >> 6) << 8) | 0x5c)

#define PFBYTE(SYM, X, I) HIDDEN(SYM##_bcs##I = BCOB_UNIT((X) >> ((I)*8)))
#define PFSTUB2(SYM, X) \
  HIDDEN(SYM = (X));    \
  PFBYTE(SYM, X, 0);    \
  PFBYTE(SYM, X, 1)
#define PFSTUB4(SYM, X) \
  PFSTUB2(SYM, X);      \
  PFBYTE(SYM, X, 2);    \
  PFBYTE(SYM, X, 3)
#define PFSTUB8(SYM, X) \
  PFSTUB4(SYM, X);      \
  PFBYTE(SYM, X, 4);    \
  PFBYTE(SYM, X, 5);    \
  PFBYTE(SYM, X, 6);    \
  PFBYTE(SYM, X, 7)

/**
 * Binary coded decimal support.
 *
 * <p>This allows linker scripts to generate dd commands, e.g. ape.lds.
 * There are a few ways to pad each number to the necessary 8 bytes.
 * Spaces cannot be prepended because busybox refuses to parse them.
 * Zeros cannot be prepended because Mac will take numbers as octal.
 * That leaves appending spaces. The user's shell ought to treat any
 * unquoted run of spaces as if there was only one, so this is safe.
 */
#define SHSTUB2(SYM, X)             \
  HIDDEN(SYM##_bcs0 = BCD_LEFT(X)); \
  HIDDEN(SYM##_bcs1 = BCD_RIGHT(X))
#define BCD_SMEAR(X) ((X) + (X) * 10000)
#define BCD_LEFT(X)                                      \
  (((X)) < 10000     ? BCD_RIGHT(BCD_SMEAR(X)) | 0x10    \
   : (X) < 100000    ? BCD_RIGHT(BCD_SMEAR((X) / 10))    \
   : (X) < 1000000   ? BCD_RIGHT(BCD_SMEAR((X) / 100))   \
   : (X) < 10000000  ? BCD_RIGHT(BCD_SMEAR((X) / 1000))  \
   : (X) < 100000000 ? BCD_RIGHT(BCD_SMEAR((X) / 10000)) \
                     : 0xffffffffffffffff)
#define BCD_RIGHT(X) \
  (((X)) < 10000     ? 0x20202020                  \
   : (X) < 100000    ? 0x20202030 +                \
                       (X) % 10                    \
   : (X) < 1000000   ? 0x20203030 +                \
                       ((X) / 10) % 10 +           \
                       (X) % 10 * 0x100            \
   : (X) < 10000000  ? 0x20303030 +                \
                       ((X) / 100) % 10 +          \
                       ((X) / 10) % 10 * 0x100 +   \
                       (X) % 10 * 0x10000          \
   : (X) < 100000000 ? 0x30303030 +                \
                       ((X) / 1000) % 10 +         \
                       ((X) / 100) % 10 * 0x100 +  \
                       ((X) / 10) % 10 * 0x10000 + \
                       (X) % 10 * 0x1000000        \
                     : 0xffffffffffffffff)

/**
 * Laying out the GDT entries for a TSS for bare metal operation.
 */
#define TSSDESCSTUB2(SYM, BASE, LIM)                 \
  HIDDEN(SYM##_desc_ent0 = TSSDESC_ENT0(BASE, LIM)); \
  HIDDEN(SYM##_desc_ent1 = TSSDESC_ENT1(BASE));      \
  ASSERT((LIM) >= 0 && (LIM) <= 0xffff, "bare metal TSS is suspiciously fat")
#define TSSDESC_ENT0(BASE, LIM)                \
  (((LIM)        <<  0 & 0x000000000000ffff) | \
   ((BASE)       << 16 & 0x000000ffffff0000) | \
    0x89         << 40                       | \
   ((LIM)  >> 16 << 48 & 0x000f000000000000) | \
    0x2          << 52                       | \
   ((BASE) >> 24 << 56 & 0xff00000000000000))
#define TSSDESC_ENT1(BASE)                     \
   ((BASE) >> 32 <<  0 & 0x00000000ffffffff)

#endif /* __ASSEMBLER__ */
#endif /* APE_MACROS_H_ */
