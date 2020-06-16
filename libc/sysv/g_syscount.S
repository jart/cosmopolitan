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

/	RII System Five system call counter.
/
/	Referencing this symbol will cause systemfive() to be hooked
/	for the purpose of counting non-Windows system calls. Please
/	note wrappers may still short circuit calls sometimes, which
/	wouldn't impact this counter.
	.bss
	.align	8
g_syscount:
	.quad	0
	.endobj	g_syscount,globl
	.previous

	.initbss 701,_init_g_syscount
g_syscount_next:
	.quad	0
	.endobj	g_syscount_next
	.previous

syscount:
	incq	g_syscount(%rip)
	jmp	*g_syscount_next(%rip)
	.endfn	syscount
	.previous

	.init.start 701,_init_g_syscount
	mov	systemfive(%rip),%rax
	stosq
	ezlea	syscount,ax
	mov	%rax,systemfive(%rip)
	.init.end 701,_init_g_syscount
