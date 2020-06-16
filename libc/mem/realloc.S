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

/	Allocates / resizes / frees memory, e.g.
/
/	Returns a pointer to a chunk of size n that contains the same data as
/	does chunk p up to the minimum of (n, p's size) bytes, or null if no
/	space is available.
/
/	If p is NULL, realloc is equivalent to malloc.
/	If p is not NULL and n is 0, realloc is equivalent to free.
/
/	The returned pointer may or may not be the same as p. The algorithm
/	prefers extending p in most cases when possible, otherwise it employs
/	the equivalent of a malloc-copy-free sequence.
/
/	Please note that p is NOT free()'d should realloc() fail, thus:
/
/	  if ((p2 = realloc(p, n2))) {
/	    p = p2;
/	    ...
/	  } else {
/	    ...
/	  }
/
/	if n is for fewer bytes than already held by p, the newly unused
/	space is lopped off and freed if possible.
/
/	The old unix realloc convention of allowing the last-free'd chunk to
/	be used as an argument to realloc is not supported.
/
/	@param	rdi (p) is address of current allocation or NULL
/	@param	rsi (n) is number of bytes needed
/	@return	rax is result, or NULL w/ errno w/o free(p)
/	@note	realloc(p=0, n=0) → malloc(32)
/	@note	realloc(p≠0, n=0) → free(p)
/	@see	dlrealloc()
realloc:jmp	*hook$realloc(%rip)
	.endfn	realloc,globl
