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
#include "libc/notice.inc"
.source	__FILE__

/	Resizes the space allocated for p to size n, only if this can be
/	done without moving p (i.e., only if there is adjacent space
/	available if n is greater than p's current allocated size, or n
/	is less than or equal to p's size). This may be used instead of
/	plain realloc if an alternative allocation strategy is needed
/	upon failure to expand space, for example, reallocation of a
/	buffer that must be memory-aligned or cleared. You can use
/	realloc_in_place to trigger these alternatives only when needed.
/
/	@param	rdi (p) is address of current allocation
/	@param	rsi (newsize) is number of bytes needed
/	@return	rax is result, or NULL w/ errno
/	@see	dlrealloc_in_place()
realloc_in_place:
	jmp	*hook$realloc_in_place(%rip)
	.endfn	realloc_in_place,globl
