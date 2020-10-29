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
#include "libc/notice.inc"
#include "libc/runtime/internal.h"
.section .start,"ax",@progbits
.source	__FILE__

	nop

/	System Five userspace program entrypoint.
/
/	@param	rsp is [n,argv₀..argvₙ₋₁,0,envp₀..,0,auxv₀..,0,..]
/	@note	FreeBSD is special (see freebsd/lib/csu/amd64/...)
/	@noreturn
_start:	test	%rdi,%rdi
	cmovnz	%rdi,%rsp
	jz	0f
	movb	$FREEBSD,hostos(%rip)
0:	mov	(%rsp),%ebx			# argc
	lea	8(%rsp),%rsi			# argv
	lea	24(%rsp,%rbx,8),%rdx		# envp
	.frame0
/	bofram	9f
	.weak	idata.iat
	.weak	idata.iatend
	ezlea	missingno,ax			# make win32 imps noop
	ezlea	idata.iat,di
	ezlea	idata.iatend,cx
	sub	%rdi,%rcx
	shr	$3,%ecx
	rep stosq
	xor	%eax,%eax			# find end of environ
	or	$-1,%ecx
	mov	%rdx,%rdi
	repnz scasq
	mov	%rdi,%rcx			# auxv
	mov	%ebx,%edi
	call	_executive
9:	ud2
	.endfn	_start,weak,hidden

/	Macintosh userspace program entrypoint.
/
/	@param	rsp is [n,argv₀..argvₙ₋₁,0,envp₀..,0,auxv₀..,0,..]
/	@note	FreeBSD is special (see freebsd/lib/csu/amd64/...)
/	@noreturn
_start_xnu:
	movb	$XNU,hostos(%rip)
	jmp	0b
	.endfn	_start_xnu,weak,hidden
