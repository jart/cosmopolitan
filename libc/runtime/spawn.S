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
#include "libc/dce.h"
#include "libc/sysv/consts/prot.h"
#include "libc/macros.h"

/	Self-bootstraps process upon existence before calling main.
_spawn:	push	%rbp
	mov	%rsp,%rbp

/	Tune FPU settings if -ffast-math is somehow used systemically.
#ifdef __FAST_MATH__
	call	__fast_math
#endif

/	Call decentralized initialization assembly.
	call	_init
#if IsModeDbg()
	call	_init				# _init() is idempotent
#endif

/	Call global initialization functions.
	call	_construct

/	Restricts .initbss memory so it's read-only after initialization.
/	TODO: Delete this unless there's measurable performance advantage.
#if !IsTrustworthy()
	mov	$PROT_READ,%edi
	call	_piro
#endif

	pop	%rbp
	ret
	.endfn	_spawn,globl
