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
.align	16

/	Mixes audio.
/
/	@param	rdi is # aligned int16[16] sample chunks to process
/	@param	rsi points to aligned pcm s16le input/output memory
/	@param	rdx points to aligned pcm s16le [0..1] input memory
sad16x8n:
	.leafprologue
	.profilable
	test	%rdi,%rdi
	jz	1f
	shl	$3,%rdi
0:	sub	$8,%rdi
	movdqa	(%rsi,%rdi,2),%xmm0
	paddsw	(%rdx,%rdi,2),%xmm0
	movdqa	%xmm0,(%rsi,%rdi,2)
	jnz	0b
1:	.leafepilogue
	.endfn	sad16x8n,globl,hidden
	.source	__FILE__
