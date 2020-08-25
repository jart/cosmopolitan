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

/	Returns 𝑥 % 1,000,000,000.
/
/	@param	rdi int64 𝑥
/	@return	rax has remainder
rem1000000000int64:
	movabs	$0x112e0be826d694b3,%rdx
	mov	%rdi,%rax
	imul	%rdx
	mov	%rdx,%rax
	sar	$0x1a,%rax
	mov	%rdi,%rdx
	sar	$0x3f,%rdx
	sub	%rdx,%rax
	imul	$0x3b9aca00,%rax,%rax
	sub	%rax,%rdi
	mov	%rdi,%rax
	ret
	.endfn	rem1000000000int64,globl
	.source	__FILE__
