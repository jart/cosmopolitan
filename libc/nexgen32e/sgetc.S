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
#include "libc/nexgen32e/uart.h"
#include "libc/notice.inc"
.real
.code16 # ∩ .code32 ∩ .code64

/	Receives byte over serial line.
/
/	This is both blocking and asynchronous.
/
/	@param	di serial port
/	@return	ax character received
/	@mode	long,legacy,real
/	@see	ttytxr
sgetc:	push	%bp
	mov	%sp,%bp
	mov	%di,%dx
	add	$UART_LSR,%dx
	mov	$UART_TTYDA,%ah
1:	in	%dx,%al
	and	%ah,%al
	rep nop				# todo(jart): interrupts are better
	jz	1b
	mov	%di,%dx
	xor	%ax,%ax
	in	%dx,%al
	pop	%bp
	ret
	.endfn	sgetc,globl
	.source	__FILE__
