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

/	Frees memory the C++ way.
/
/	@param	%rdi is pointer, or NULL for no-op
/	@param	%rsi is ignored
/	@param	%rdx is ignored
_ZdlPvSt11align_val_tRKSt9nothrow_t:
/	operator delete(void*, std::align_val_t, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvSt11align_val_tRKSt9nothrow_t,weak
_ZdaPvSt11align_val_tRKSt9nothrow_t:
/	operator delete[](void*, std::align_val_t, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvSt11align_val_tRKSt9nothrow_t,weak
_ZdlPvRKSt9nothrow_t:
/	operator delete(void*, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvRKSt9nothrow_t,weak
_ZdaPvRKSt9nothrow_t:
/	operator delete[](void*, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvRKSt9nothrow_t,weak
_ZdlPvmSt11align_val_t:
/	operator delete(void*, unsigned long, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvmSt11align_val_t,weak
_ZdaPvmSt11align_val_t:
/	operator delete[](void*, unsigned long, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvmSt11align_val_t,weak
_ZdlPvSt11align_val_t:
/	operator delete(void*, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvSt11align_val_t,weak
_ZdaPvSt11align_val_t:
/	operator delete[](void*, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvSt11align_val_t,weak
_ZdaPvm:	
/	operator delete[](void*, unsigned long):
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvm,weak
_ZdlPvm:
/	operator delete(void*, unsigned long)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvm,weak
_ZdaPv:
/	operator delete[](void*)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPv,weak
_ZdlPv:
/	operator delete(void*)
	jmp	*hook$free(%rip)
	.endfn	_ZdlPv,weak
