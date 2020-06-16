/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"

#define BIAS 0x84

/	Encodes audio sample with µ-Law.
/
/	This is both the highest quality and most widely supported
/	telephony codec, whose use was phased out in the 2000's in
/	favor of cost-saving GSM audio compression that was so bad
/	consumers were willing to pay more cash, for the privilege
/	of saving telcos even more money w/ text messaging. Mu Law
/	reduces PCM data to half its original size, by diminishing
/	audio bands not vocalized by human voice.
/
/	@param	%edi is pcm sample
/	@return	%eax is uint8_t encoded sample
mulaw:	.leafprologue
	.profilable
	mov	$BIAS,%eax
	xor	%edx,%edx
	test	%edi,%edi
	js	1f
	lea	(%edi,%eax),%eax
	mov	$0xFF,%dl
	jmp	2f
1:	sub	%edi,%eax
	mov	$0x7F,%dl
2:	mov	%eax,%esi
	or	$0xFF,%sil
	bsr	%esi,%esi
	sub	$7,%esi
	cmp	$8,%esi
	jge	4f
	lea	3(%rdx),%ecx
	sar	%cl,%eax
	and	$0xF,%eax
	shl	$4,%esi
	or	%esi,%eax
	xor	%edx,%eax
3:	.leafepilogue
4:	xor	$0x7F,%dl
	mov	%edx,%eax
	jmp	3b
	.endfn	mulaw,globl
	.source	__FILE__
