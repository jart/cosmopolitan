/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
.text.startup
.source	__FILE__

/	Intel Manual V.1 §10.2.3 - MXCSR Control and Status Register
#define MXCSR_DE (1 << 1)	/* Denormal Flag */
#define MXCSR_DAZ (1 << 6)	/* Denormals Are Zeros */
#define MXCSR_DM (1 << 8)	/* Denormal Operation Mask */
#define MXCSR_FTZ (1 << 15)	/* Flush to Zero */

/	Initializes fast math.
/
/	 “Seymour Cray didn't care that 81.0/3.0 did not give exactly
/	  27.0 on the CDC 6000 class machines, and he was universally
/	  respected for making the fastest machines around.
/	                                        ──Linus Torvalds
__fast_math:
	push	%rbp
	mov	%rsp,%rbp
	push	%rax
	stmxcsr	(%rsp)
	orl	$MXCSR_FTZ+MXCSR_DAZ,(%rsp)
	ldmxcsr	(%rsp)
	leave
	ret
	.endfn	__fast_math,globl
