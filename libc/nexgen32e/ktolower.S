/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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
#include "libc/macros.h"

/	ASCII uppercase → lowercase translation tables.
/
/		char kToLower[256];
/		char16_t kToLower16[256];
/
/	@note	kToLower16 saves 128kb; CMOVcc can't even 8-bit
	.initbss 300,_init_kToLower
kToLower:
	.rept	256
	.byte	0
	.endr
	.endobj	kToLower,globl,hidden
kToLower16:
	.rept	256
	.short	0
	.endr
	.endobj	kToLower16,globl,hidden
	.previous

	.init.start 300,_init_kToLower
	push	%rdi
	call	imapxlatab
	xchg	%rsi,(%rsp)
	xor	%ecx,%ecx
0:	inc	%ecx
	addb	$0x20,'A-1(%rsi,%rcx)
	cmp	$'Z-'A,%ecx
	jne	0b
	xor	%eax,%eax
	mov	$256,%ecx
0:	lodsb
	stosw
	loop	0b
	pop	%rsi
	.init.end 300,_init_kToLower

	.type	gperf_downcase,@object
	.globl	gperf_downcase
	gperf_downcase = kToLower
	.yoink	__FILE__
