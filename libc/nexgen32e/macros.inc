/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"

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
