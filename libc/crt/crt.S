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
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/notice.inc"
#include "libc/runtime/internal.h"
.section .start,"ax",@progbits
.yoink	__FILE__

/	System Five userspace program entrypoint.
/
/	@param	rsp is [n,argv₀..argvₙ₋₁,0,envp₀..,0,auxv₀..,0,..]
/	@note	FreeBSD is special (see freebsd/lib/csu/amd64/...)
/	@noreturn
_start_xnu:
	movb	$XNU,hostos(%rip)
	jmp	0f
_start:	test	%rdi,%rdi
	cmovnz	%rdi,%rsp
	jz	0f
	movb	$FREEBSD,hostos(%rip)
0:	movslq	(%rsp),%r12			# argc
	lea	8(%rsp),%r13			# argv
	lea	24(%rsp,%r12,8),%r14		# envp
	.weak	idata.iat,idata.iatend
	ezlea	missingno,ax			# make win32 imps noop
	ezlea	idata.iat,di
	ezlea	idata.iatend,cx
	sub	%rdi,%rcx
	shr	$3,%ecx
	rep stosq
	xor	%eax,%eax			# find end of environ
	or	$-1,%ecx
	mov	%r14,%rdi
	repnz scasq
	mov	%rdi,%r15			# auxv
	jmp	__executive
	.endfn	_start,weak,hidden
	.endfn	_start_xnu,weak,hidden
