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
#include "libc/macros.h"
.source	__FILE__

/	RII System Five system call counter.
/
/	Referencing this symbol will cause systemfive() to be hooked
/	for the purpose of counting non-Windows system calls. Please
/	note wrappers may still short circuit calls sometimes, which
/	wouldn't impact this counter.
	.bss
	.align	8
g_syscount:
	.quad	0
	.endobj	g_syscount,globl
	.previous

	.initbss 701,_init_g_syscount
g_syscount_next:
	.quad	0
	.endobj	g_syscount_next
	.previous

syscount:
	incq	g_syscount(%rip)
	jmp	*g_syscount_next(%rip)
	.endfn	syscount
	.previous

	.init.start 701,_init_g_syscount
	mov	__systemfive(%rip),%rax
	stosq
	ezlea	syscount,ax
	mov	%rax,__systemfive(%rip)
	.init.end 701,_init_g_syscount
