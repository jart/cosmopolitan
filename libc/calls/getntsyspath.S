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

/	Obtains WIN32 magic path, e.g. GetTempPathA.
/
/	@param	rax is address of ANSI path provider function
/	@param	rdi is output buffer
/	@param	rdx is output buffer size in bytes that's >0
/	@return	eax is string length w/ NUL that's ≤ edx
/	@return	rdi is rdi+edx
	.text.startup
__getntsyspath:
	push	%rbp
	mov	%rsp,%rbp
	push	%rdx
	movpp	%rdi,%rcx		# call f=%rax(p1=%rcx,p2=%rdx)
	sub	$40,%rsp
	call	*%rax
	xor	%edx,%edx
	mov	-8(%rbp),%ecx		# restore %edx param as %ecx
	cmp	%eax,%ecx		# use current dir on overflow
	cmovbe	%edx,%eax
	cmp	$1,%eax			# leave empty strings empty
	jbe	1f
	cmpb	$'\\,-1(%rdi,%rax)	# guarantee trailing slash
	je	1f
	movw	$'\\,(%rdi,%rax)
	inc	%eax
1:	inc	%rdi			# change backslash to slash
	cmpb	$'\\,-1(%rdi)
	jne	2f
	movb	$'/,-1(%rdi)
2:	loop	1b
	leave
	ret
	.endfn	__getntsyspath,globl,hidden
