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
.yoink	__FILE__

/	Creates file descriptors for IPC.
/
/	@param	rdi points to int3[2] that gets (reader, writer)
/	@return 0 on success or -1 w/ errno
/	@asyncsignalsafe
/	@see	libc/sysv/syscalls.sh
/	@see	pipe2()
pipe$sysv:
	push	%rbp
	mov	%rsp,%rbp
	xor	%esi,%esi			# FreeBSD is pipe2()
	call	__pipe$sysv
#if SupportsXnu()
	testb	IsXnu()				# XNU has special needs
	jz	1f
	cmp	$-1,%eax
	je	1f
	mov	%eax,(%rdi)
	mov	%edx,4(%rdi)
	xor	%eax,%eax
#endif
1:	pop	%rbp
	ret
	.endfn	pipe$sysv,globl,hidden
