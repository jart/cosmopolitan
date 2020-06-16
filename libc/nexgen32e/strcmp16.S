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

/	Compares NUL-terminated char16_t strings.
/
/	@param	rdi is first non-null NUL-terminated char16_t string
/	@param	rsi is second non-null NUL-terminated char16_t string
/	@param	rdx is maximum shorts to consider
/	@return	rax is <0, 0, or >0 based on uint16_t comparison
/	@note	cosmopolitan headers optimize away this indirection
/	@asyncsignalsafe
strcmp16:
	jmp	*hook$strcmp16(%rip)
	.endfn	strcmp16,globl
	.source	__FILE__
