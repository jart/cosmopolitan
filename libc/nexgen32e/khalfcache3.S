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

/	Half size of level 3 cache in bytes.
	.initbss 202,_init_kHalfCache3
kHalfCache3:
	.quad	0
	.endobj	kHalfCache3,globl
	.previous

	.init.start 202,_init_kHalfCache3
	cmpl	$3,kCpuids(%rip)
	jbe	3f
	xor	%r8d,%r8d
	mov	$4,%r8d
1:	mov	%r8d,%eax
	mov	%r8d,%ecx
	push	%rbx
	cpuid
	mov	%ebx,%r9d
	pop	%rbx
	test	$31,%al
	je	3f
	cmp	$99,%al
	jne	2f
	mov	%r9d,%eax
	mov	%r9d,%edx
	inc	%ecx
	shr	$12,%r9d
	shr	$22,%eax
	and	$0x0fff,%edx
	and	$0x03ff,%r9d
	inc	%eax
	inc	%edx
	imul	%edx,%eax
	imul	%ecx,%eax
	lea	1(%r9),%ecx
	imul	%ecx,%eax
	jmp	4f
2:	inc	%r8d
	jmp	1b
3:	mov	$0x00400000,%eax
4:	shr	%eax
	stosq
	.init.end 202,_init_kHalfCache3
	.source	__FILE__
