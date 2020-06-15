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
.privileged

/	Loads XMM registers from buffer.
/
/	@param	%rdi points to &(aligned(16) uint8_t[256])[128]
/	@note	modern cpus have out-of-order execution engines
loadxmm:.leafprologue
	movaps	-0x80(%rdi),%xmm0
	movaps	-0x70(%rdi),%xmm1
	movaps	-0x60(%rdi),%xmm2
	movaps	-0x50(%rdi),%xmm3
	movaps	-0x40(%rdi),%xmm4
	movaps	-0x30(%rdi),%xmm5
	movaps	-0x20(%rdi),%xmm6
	movaps	-0x10(%rdi),%xmm7
	movaps	0x00(%rdi),%xmm8
	movaps	0x10(%rdi),%xmm9
	movaps	0x20(%rdi),%xmm10
	movaps	0x30(%rdi),%xmm11
	movaps	0x40(%rdi),%xmm12
	movaps	0x50(%rdi),%xmm13
	movaps	0x60(%rdi),%xmm14
	movaps	0x70(%rdi),%xmm15
	.leafepilogue
	.endfn	loadxmm,globl,hidden
	.yoink	__FILE__
