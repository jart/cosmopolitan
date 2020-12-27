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
.source	__FILE__

/	Allocates memory the C++ way.
/
/	\param	%rdi is bytes to allocate
/	\param	%rsi is ignored
/	\return	new memory or NULL on OOM
_ZnamRKSt9nothrow_t:
/	operator new[](unsigned long, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZnamRKSt9nothrow_t,weak
_ZnwmRKSt9nothrow_t:
/	operator new(unsigned long, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZnwmRKSt9nothrow_t,weak
_Znam:
/	operator new[](unsigned long)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_Znam,weak
_Znwm:
/	operator new(unsigned long)
	test	%rdi,%rdi
	jne	1f
	mov	$1,%edi
1:	jmp	*hook$malloc(%rip)
	.endfn	_Znwm,weak
