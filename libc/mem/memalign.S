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
#include "libc/notice.inc"
.source	__FILE__

/	Allocates aligned memory.
/
/	Returns a pointer to a newly allocated chunk of n bytes, aligned in
/	accord with the alignment argument. The alignment argument should be
/	a power of two. If the argument is not a power of two, the nearest
/	greater power is used. 8-byte alignment is guaranteed by normal
/	malloc calls, so don't bother calling memalign with an argument of 8
/	or less.
/
/	@param	rdi is alignment in bytes
/	@param	rsi (newsize) is number of bytes needed
/	@return	rax is memory address, or NULL w/ errno
/	@note	overreliance on memalign is a sure way to fragment space
/	@see	dlmemalign()
memalign:
	jmp	*hook$memalign(%rip)
	.endfn	memalign,globl
