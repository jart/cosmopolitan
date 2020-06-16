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

/	Returns the number of bytes you can actually use in
/	an allocated chunk, which may be more than you requested
/	(although often not) due to alignment and minimum size
/	constraints.
/
/	You can use this many bytes without worrying about overwriting
/	other allocated objects. This is not a particularly great
/	programming practice. malloc_usable_size can be more useful in
/	debugging and assertions, for example:
/
/	    p = malloc(n)
/	    assert(malloc_usable_size(p) >= 256)
/
/	@param	rdi is address of allocation
/	@return	rax is total number of bytes
/	@see	dlmalloc_usable_size()
malloc_usable_size:
	jmp	*hook$malloc_usable_size(%rip)
	.endfn	malloc_usable_size,globl

	.initbss 800,_init_malloc_usable_size
hook$malloc_usable_size:
	.quad	0
	.endobj	hook$malloc_usable_size,globl,hidden
	.previous

	.init.start 800,_init_malloc_usable_size
	ezlea	dlmalloc_usable_size,ax
	stosq
	.init.end 800,_init_malloc_usable_size
