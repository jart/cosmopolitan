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
	nop
_ZdaPvSt11align_val_tRKSt9nothrow_t:
	nop
_ZdlPvRKSt9nothrow_t:
	nop
_ZdaPvRKSt9nothrow_t:
	nop
_ZdlPvmSt11align_val_t:
	nop
_ZdaPvmSt11align_val_t:
	nop
_ZdlPvSt11align_val_t:
	nop
_ZdaPvSt11align_val_t:
	nop
_ZdaPvm:nop
_ZdlPvm:nop
_ZdaPv:	nop
_ZdlPv:	jmp	*hook$free(%rip)
	.endfn	_ZdlPv,globl,weak
	.endfn	_ZdaPv,globl,weak
	.endfn	_ZdaPvm,globl,weak
	.endfn	_ZdlPvm,globl,weak
	.endfn	_ZdaPvRKSt9nothrow_t,globl,weak
	.endfn	_ZdlPvRKSt9nothrow_t,globl,weak
	.endfn	_ZdaPvSt11align_val_t,globl,weak
	.endfn	_ZdlPvSt11align_val_t,globl,weak
	.endfn	_ZdaPvmSt11align_val_t,globl,weak
	.endfn	_ZdlPvmSt11align_val_t,globl,weak
	.endfn	_ZdlPvSt11align_val_tRKSt9nothrow_t,globl,weak
	.endfn	_ZdaPvSt11align_val_tRKSt9nothrow_t,globl,weak
