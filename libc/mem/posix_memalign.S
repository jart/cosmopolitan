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
.yoink	__FILE__

/	Allocates aligned memory the POSIX way.
/
/	Allocates a chunk of n bytes, aligned in accord with the alignment
/	argument. Differs from memalign only in that it (1) assigns the
/	allocated memory to *pp rather than returning it, (2) fails and
/	returns EINVAL if the alignment is not a power of two (3) fails and
/	returns ENOMEM if memory cannot be allocated.
/
/	@param	rdi is void **pp
/	@param	rsi is size_t align
/	@param	rdx is size_t size
/	@return	eax
posix_memalign:
	jmp	*hook$posix_memalign(%rip)
	.endfn	posix_memalign,globl

	.initbss 800,_init_posix_memalign
hook$posix_memalign:
	.quad	0
	.endobj	hook$posix_memalign,globl,hidden
	.previous

	.init.start 800,_init_posix_memalign
	.hidden	dlposix_memalign
	ezlea	dlposix_memalign,ax
	stosq
	.init.end 800,_init_posix_memalign

	yoink	free
