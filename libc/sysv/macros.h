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
#ifndef COSMOPOLITAN_LIBC_SYSV_MACROS_H_
#define COSMOPOLITAN_LIBC_SYSV_MACROS_H_
#ifdef __ASSEMBLER__
#include "libc/macros.inc"
/* clang-format off */

/**
 * @fileoverview System Five jump slot generator.
 *
 * We need to pass a magic number to the SYSCALL instruction in %rax.
 * That's only possible to do in C using asm() macro wrappers, like in
 * Chromium's "System Call Support" library. This technique is simpler
 * to implement, gains us object-level ABI compatibility, hookability,
 * and the ability to support significantly more functions, without the
 * risk of slowing down builds too much with complicated headers.
 */

.macro	.scall	name:req num:req kw1 kw2
  .ifnb	\kw2
	.align	16
\name:	movabs	$\num,%rax
	jmp	*systemfive(%rip)
  .else
\name:	push	%rbp
	mov	%rsp,%rbp
	movabs	$\num,%rax
	.hookable
	call	*systemfive(%rip)
	pop	%rbp
	ret
  .endif
	.endfn	\name,\kw1,\kw2
	.previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_SYSV_MACROS_H_ */
