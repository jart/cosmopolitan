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
#include "ape/macros.internal.h"
.real
.source	__FILE__
.code16 # ∩ .code32 ∩ .code64

/	Linkable delegate for traditional Unix user process termination.
/
/	@param	edi is exit code ∈ [0,256)
/	@note	cosmopolitan headers obfuscate this indirection
/	@see	libc/runtime/runtime.h
/	@mode	long,legacy,real
/	@asyncsignalsafe
/	@noreturn
_exit:	push	%bp
	mov	%sp,%bp
	.weak	_Exit
	rlcall	_Exit
	int3
	.endfn	_exit,weak
