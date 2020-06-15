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
#include "ape/macros.h"
.real
.yoink	__FILE__
.code16 # ∩ .code32 ∩ .code64

/**
 * @fileoverview Overridable stubs for synthetic stack protector calls.
 */

__stack_chk_fail_local:
	nop
/	fallthrough
	.endfn	__stack_chk_fail_local,weak,hidden

__stack_chk_fail:
	push	%bp
	mov	%sp,%bp
	rlcall	abort
	nop
	.endfn	__stack_chk_fail,weak

	.bss
	.align	8
__stack_chk_guard:
	.quad	0
	.endobj	__stack_chk_guard,weak
	.previous
