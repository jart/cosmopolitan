/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nt/enum/status.h"
#include "libc/macros.h"

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
