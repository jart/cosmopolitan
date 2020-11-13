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
#include "libc/macros.h"

/	Forks process without copying page tables.
/
/	This is the same as fork() except it's optimized for the case
/	where the caller invokes exec() immediately afterwards.
/
/	@return	pid of child process or 0 if forked process
/	@returnstwice
vfork:	testb	IsWindows()
	jnz	fork$nt
	mov	__NR_vfork(%rip),%eax
	cmp	$-1,%eax
	je	systemfive.enosys
	pop	%rsi
	testb	IsBsd()
	jnz	vfork.bsd
	syscall
	push	%rsi
	cmp	$-4095,%rax
	jae	systemfive.error
	ret
	.endfn	vfork,globl

vfork.bsd:
	syscall
	push	%rsi
	jc	systemfive.errno
	ret
	.endfn	vfork.bsd
