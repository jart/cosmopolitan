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

/	Performs addition on binary-coded decimals.
bcdadd:	.leafprologue
	.profilable
	lea	0x6666666(%rdi),%ecx
	xor	%esi,%ecx
	lea	(%rdi,%rsi),%eax
	add	$0x6666666,%eax
	xor	%eax,%ecx
	not	%ecx
	and	$0x11111110,%ecx
	mov	%ecx,%edx
	shr	$2,%edx
	shr	$3,%ecx
	orl	%edx,%ecx
	sub	%ecx,%eax
	.leafepilogue
	.endfn	bcdadd,globl
