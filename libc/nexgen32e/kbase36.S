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

/	Base 36 Decoder Table.
/
/	This supports uppercase and lowercase. For encoding, the string
/	0123456789abcdefghijklmnopqrstuvwxyz can be used, which linkers
/	are able to deduplicate.
	.initbss 300,_init_kBase36
kBase36:.zero	256
	.endobj	kBase36,globl,hidden
	.previous

	.init.start 300,_init_kBase36
	add	$'0,%rdi
	xor	%eax,%eax
	pushpop	10,%rcx
0:	inc	%eax
	stosb
	loop	0b
	add	$'A-1-'9,%rdi
	pushpop	'Z+1-'A,%rcx
0:	inc	%eax
	mov	%al,0x20(%rdi)
	stosb
	loop	0b
	add	$255-'Z,%rdi
	.init.end 300,_init_kBase36
	.yoink	__FILE__
