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
#include "ape/macros.h"
#include "ape/config.h"
.real
.source	__FILE__
.code16 # ∩ .code32 ∩ .code64

/	Hoses interrupt descriptor table and triple-faults the system.
/
/	@see	youtu.be/GIKfEAF2Yhw?t=67
/	@mode	long,legacy,real
triplf:	push	%bp
	mov	%sp,%bp
	sub	$8,%sp
	movpp	%bp,%si
	lea	-8(%bp),%di
	pushpop	8,%cx
	xor	%ax,%ax
	rep stosb
0:	cli
	lidt	-8(%bp)
	ud2
	jmp	0b
	.endfn	triplf,globl,protected
