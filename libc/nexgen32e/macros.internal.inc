/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"
#include "libc/nexgen32e/x86feature.h"

/	Broadcast byte literal to vector, e.g.
/
/		# xmm0=0x12121212121212121212121212121212
/		.bcblit	$0x12,%al,%eax,%xmm0
/
/	@param	reg and regSI need to be the same register
	.macro	.bcblit	lit:req reg:req regSI:req xmm:req
	mov	\lit,\reg
	movd	\regSI,\xmm
	pbroadcastb \xmm
	.endm

/	Broadcast word literal to vector, e.g.
/
/		# xmm0=0x01230123012301230123012301230123
/		.bcwlit	$0x123,%ax,%eax,%xmm0
/
/	@param	reg and regSI need to be the same register
	.macro	.bcwlit	lit:req reg:req regSI:req xmm:req
	mov	\lit,\reg
	movd	\regSI,\xmm
	pbroadcastw \xmm
	.endm

/	Broadcast int16 from register to vector.
	.macro	.bcwreg	regSI:req xmm:req
	movd	\regSI,\xmm
	pbroadcastw \xmm
	.endm

/	Sets all bytes in XMM register to first byte, e.g.
/
/		mov	$0x11,%eax
/		movd	%eax,%xmm0
/		pbroadcastb %xmm0
/
/		  11000000000000000000000000000000
/		→ 11111111111111111111111111111111
/
/	@param	xmm can be %xmm0,%xmm1,etc.
	.macro	pbroadcastb xmm:req
#if X86_NEED(AVX2)
	vpbroadcastb \xmm,\xmm
#else
	punpcklbw \xmm,\xmm
	punpcklwd \xmm,\xmm
	pshufd	$0,\xmm,\xmm
#endif
	.endm

/	Sets all words in XMM register to first word, e.g.
/
/		mov	$0x1234,%eax
/		movd	%eax,%xmm0
/		pbroadcastw %xmm0
/
/		  12340000000000000000000000000000
/		→ 12341234123412341234123412341234
/
/	@param	xmm can be %xmm0,%xmm1,etc.
	.macro	pbroadcastw xmm:req
#if X86_NEED(AVX2)
	vpbroadcastw \xmm,\xmm
#else
	punpcklwd \xmm,\xmm
	pshufd	$0,\xmm,\xmm
#endif
	.endm
