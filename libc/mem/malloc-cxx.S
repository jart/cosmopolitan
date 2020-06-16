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

/	Allocates memory the C++ way.
/
/	@param	%rdi is bytes to allocate
/	@param	%rsi is ignored
/	@return	new memory or NULL on OOM
_ZnamRKSt9nothrow_t:
	nop
_ZnwmRKSt9nothrow_t:
	nop
_Znam:	nop
_Znwm:	test	%rdi,%rdi
	jne	1f
	mov	$1,%edi
1:	jmp	*hook$malloc(%rip)
	.endfn	_Znwm,globl,weak
	.endfn	_Znam,globl,weak
	.endfn	_ZnwmRKSt9nothrow_t,globl,weak
	.endfn	_ZnamRKSt9nothrow_t,globl,weak
