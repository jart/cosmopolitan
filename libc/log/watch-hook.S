/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"

__watch_hook:
	push	%rbp
	mov	%rsp,%rbp
	and	$-16,%rsp
	sub	$0x80,%rsp
	movaps	%xmm0,0x00(%rsp)
	movaps	%xmm1,0x10(%rsp)
	movaps	%xmm2,0x20(%rsp)
	movaps	%xmm3,0x30(%rsp)
	movaps	%xmm4,0x40(%rsp)
	movaps	%xmm5,0x50(%rsp)
	movaps	%xmm6,0x60(%rsp)
	movaps	%xmm7,0x70(%rsp)
	push	%rax
	push	%rax
	push	%rdi
	push	%rsi
	push	%rdx
	push	%rcx
	push	%r8
	push	%r9
	push	%r10
	push	%r11
	call	__watcher
	pop	%r11
	pop	%r10
	pop	%r9
	pop	%r8
	pop	%rcx
	pop	%rdx
	pop	%rsi
	pop	%rdi
	pop	%rax
	pop	%rax
	movaps	0x00(%rsp),%xmm0
	movaps	0x10(%rsp),%xmm1
	movaps	0x20(%rsp),%xmm2
	movaps	0x30(%rsp),%xmm3
	movaps	0x40(%rsp),%xmm4
	movaps	0x50(%rsp),%xmm5
	movaps	0x60(%rsp),%xmm6
	movaps	0x70(%rsp),%xmm7
	leave
	ret
	.endfn	__watch_hook,globl
