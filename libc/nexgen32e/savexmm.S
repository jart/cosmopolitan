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

/	Stores XMM registers to buffer.
/
/	@param	%rdi points to &(aligned(16) uint8_t[256])[128]
/	@note	modern cpus have out-of-order execution engines
savexmm:.leafprologue
	movaps	%xmm0,-0x80(%rdi)
	movaps	%xmm1,-0x70(%rdi)
	movaps	%xmm2,-0x60(%rdi)
	movaps	%xmm3,-0x50(%rdi)
	movaps	%xmm4,-0x40(%rdi)
	movaps	%xmm5,-0x30(%rdi)
	movaps	%xmm6,-0x20(%rdi)
	movaps	%xmm7,-0x10(%rdi)
	movaps	%xmm8,0x00(%rdi)
	movaps	%xmm9,0x10(%rdi)
	movaps	%xmm10,0x20(%rdi)
	movaps	%xmm11,0x30(%rdi)
	movaps	%xmm12,0x40(%rdi)
	movaps	%xmm13,0x50(%rdi)
	movaps	%xmm14,0x60(%rdi)
	movaps	%xmm15,0x70(%rdi)
	.leafepilogue
	.endfn	savexmm,globl,hidden
	.yoink	__FILE__
