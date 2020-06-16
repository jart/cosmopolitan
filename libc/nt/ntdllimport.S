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
#include "libc/nt/enum/status.h"
#include "libc/macros.h"
.source	__FILE__

/	@fileoverview NTDLL.DLL Non-Mandatory Importer
/
/	This module lets us import Microsoft's private APIs in a way
/	that ensures executables won't fail to load in the future,
/	should Microsoft ever choose to delete these APIs.

	.initro 202,_init_ntdll.1
	.type	kNtdllProcRvas,@object
kNtdllProcRvas:
	.previous/*
	...
	decentralized content
	...
	*/.initro 202,_init_ntdll.3
	.quad	0
	.previous

	.init.start 202,_init_ntdll
	push	%r12
	push	%r13
	lea	ntdllmissingno(%rip),%r13
	sub	$32,%rsp
	loadstr	"ntdll.dll",cx
	call	*__imp_GetModuleHandleA(%rip)
	mov	%rax,%r12
0:	lodsq
	test	%rax,%rax
	jz	1f
	lea	(%rbx,%rax),%rdx
	mov	%r12,%rcx
	call	*__imp_GetProcAddress(%rip)
	test	%rax,%rax
	cmovz	%r13,%rax
	stosq
	jmp	0b
1:	add	$32,%rsp
	pop	%r13
	pop	%r12
	.init.end 202,_init_ntdll,globl,hidden

	.text.windows
ntdllmissingno:
	mov	$kNtStatusDllNotFound,%eax
	ret
	.previous
