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

	.code16
	.section .start,"ax",@progbits
_start:	jmp	1f
1:	ljmp	$0x600>>4,$_begin
_begin:	push	%cs
	pop	%ds
	push	%cs
	pop	%es
	mov	$0x70000>>4,%ax
	cli
	mov	%ax,%ss
	xor	%sp,%sp
	sti
	cld
	xor	%ax,%ax
	xor	%di,%di
	mov	$0x7c00-0x600,%cx
	rep stosb
	xchg	%di,%bx
	inc	%cx
	xor	%dh,%dh
	mov	$v_sectors+0x0200,%ax
	int	$0x13
	xor	%bp,%bp
	sub	$6,%sp
	call	main
	nop
	.type	_start,@function
	.size	_start,.-_start
	.globl	_start
	.globl	v_sectors
	.globl	main

setjmp:	mov	%sp,%ax
	stosw				# sp
	xchg	%ax,%si
	movsw	%ss:(%si),(%di)		# ip
	mov	%bp,%ax
	stosw				# bp
	ret
	.type	setjmp,@function
	.size	setjmp,.-setjmp
	.globl	setjmp

longjmp:
	mov	(%di),%sp
	mov	2(%di),%dx
	mov	4(%di),%bp
	pop	%ax
	mov	%si,%ax
	jmp	*%dx
	.type	longjmp,@function
	.size	longjmp,.-longjmp
	.globl	longjmp

	.globl	q.syntax
	.type	q.syntax,@function
	.globl	q.look
	.type	q.look,@function
	.globl	q.globals
	.type	q.globals,@function
	.globl	q.index
	.type	q.index,@function
	.globl	q.token
	.type	q.token,@function
	.globl	q.str
	.type	q.str,@function

	.globl	boot
	.type	boot,@function
	.globl	bss
	.type	bss,@function
	.globl	rodata
	.type	rodata,@function
