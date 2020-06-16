/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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
#include "libc/notice.inc"

/	Dispatches to fastest strcmp16() implementation.
/
/	@param	rdi is first non-null NUL-terminated char16_t string
/	@param	rsi is second non-null NUL-terminated char16_t string
/	@return	rax is <0, 0, or >0 based on uint16_t comparison
/	@asyncsignalsafe
	.initbss 300,_init_strcmp16
hook$strcmp16:
	.quad	0
	.endobj	hook$strcmp16,globl,hidden
	.previous

	.init.start 300,_init_strcmp16
	ezlea	strcmp16$k8,ax
	stosq
	.init.end 300,_init_strcmp16
	.source	__FILE__
