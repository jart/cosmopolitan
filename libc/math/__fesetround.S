/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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

fesetround:
	push	%rax
	xor	%eax,%eax
	mov	%edi,%ecx
	fnstcw	(%rsp)
	andb	$0xf3,1(%rsp)
	or	%ch,1(%rsp)
	fldcw	(%rsp)
	stmxcsr	(%rsp)
	shl	$3,%ch
	andb	$0x9f,1(%rsp)
	or	%ch,1(%rsp)
	ldmxcsr	(%rsp)
	pop	%rcx
	ret
	.endfn	fesetround,globl,hidden
	.source	__FILE__
