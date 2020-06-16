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
.source	__FILE__

/	Allocates uninitialized memory.
/
/	Returns a pointer to a newly allocated chunk of at least n bytes, or
/	null if no space is available, in which case errno is set to ENOMEM
/	on ANSI C systems.
/
/	If n is zero, malloc returns a minimum-sized chunk. (The minimum size
/	is 32 bytes on 64bit systems.) Note that size_t is an unsigned type,
/	so calls with arguments that would be negative if signed are
/	interpreted as requests for huge amounts of space, which will often
/	fail. The maximum supported value of n differs across systems, but is
/	in all cases less than the maximum representable value of a size_t.
/
/	@param	rdi is number of bytes needed
/	@return	new memory, or NULL w/ errno
/	@note	malloc(0) → malloc(32)
/	@see	dlmalloc()
malloc:	jmp	*hook$malloc(%rip)
	.endfn	malloc,globl

