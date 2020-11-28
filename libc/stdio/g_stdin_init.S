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
#include "libc/sysv/consts/o.h"
#include "libc/dce.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/macros.h"
.source	__FILE__

	.init.start 400,_init_g_stdin
	ezlea	g_stdin,ax
	mov	O_RDONLY,%edx
	mov	%edx,4(%rax)			#→ f.iomode
	ezlea	g_stdin_buf,cx
	mov	%rcx,24(%rax)			#→ f.buf
	movl	$BUFSIZ,32(%rax)		#→ f.size
	ezlea	__freadbuf,cx
	mov	%rcx,%rdx
	mov	%rdx,40(%rax)			#→ f.reader
	mov	%rax,stdin(%rip)
	.init.end 400,_init_g_stdin,globl,hidden
