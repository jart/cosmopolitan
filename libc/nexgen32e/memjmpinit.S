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
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"
.text.startup

/	Initializes jump table for memset() and memcpy().
/
/	@param	!ZF if required cpu vector extensions are available
/	@param	rdi is address of 64-bit jump table
/	@param	rsi is address of 8-bit jump initializers
/	@param	rdx is address of indirect branch
/	@param	ecx is size of jump table
memjmpinit:
	.leafprologue
	setnz	%r8b
	shl	%r8b
0:	xor	%eax,%eax
	lodsb
	add	%rdx,%rax
	stosq
	.loop	0b
	xor	%eax,%eax
	testb	X86_HAVE(ERMS)+kCpuids(%rip)
	setnz	%al
	or	%r8b,%al
	mov	(%rsi,%rax),%al
	add	%rdx,%rax
	stosq
	lodsq
	.leafepilogue
	.endfn	memjmpinit,globl,hidden
	.source	__FILE__
