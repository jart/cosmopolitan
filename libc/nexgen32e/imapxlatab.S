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
.text.startup
.align	8

/	Identity maps 256-byte translation table.
/
/	@param	char (*rdi)[256]
/	@speed	90mBps
/	@mode	long
imapxlatab:
	.leafprologue
	.profilable
 	pushpop	32,%rcx
	mov	$0x0706050403020100,%rax
	mov	$0x0808080808080808,%rdx
	.align	8
1:	stosq
	add	%rdx,%rax
	loop	1b
	.leafepilogue
	.endfn	imapxlatab,globl,hidden
	.yoink	__FILE__
