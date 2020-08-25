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

/	Support code for fast integer division by Si units.
/
/	Division by magnums is described in Hacker's Delight and is
/	usually generated automatically by compilers, but sadly not
/	when we optimize for size and idiv goes at least 10x slower
/	so we do this which saves space while avoiding build tuning
/
/	@param	rdi is number to divide
/	@param	cl is magnum #1
/	@param	rdx is magnum #2
/	@return	quotient
tinydivsi:
	.leafprologue
	mov	%rdi,%rax
	imul	%rdx
	mov	%rdx,%rax
	sar	%cl,%rax
	sar	$63,%rdi
	sub	%rdi,%rax
	.leafepilogue
	.endfn	tinydivsi,globl
	.source	__FILE__
