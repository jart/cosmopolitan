/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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

/	ASCII uppercase → lowercase translation tables.
/
/		char kToLower[256];
/		char16_t kToLower16[256];
/
/	@note	kToLower16 saves 128kb; CMOVcc can't even 8-bit
	.initbss 300,_init_kToLower
kToLower:
	.rept	256
	.byte	0
	.endr
	.endobj	kToLower,globl,hidden
kToLower16:
	.rept	256
	.short	0
	.endr
	.endobj	kToLower16,globl,hidden
	.previous

	.init.start 300,_init_kToLower
	push	%rdi
	call	imapxlatab
	xchg	%rsi,(%rsp)
	xor	%ecx,%ecx
0:	inc	%ecx
	addb	$0x20,'A-1(%rsi,%rcx)
	cmp	$'Z-'A,%ecx
	jne	0b
	xor	%eax,%eax
	mov	$256,%ecx
0:	lodsb
	stosw
	.loop	0b
	pop	%rsi
	.init.end 300,_init_kToLower

	.type	gperf_downcase,@object
	.globl	gperf_downcase
	gperf_downcase = kToLower
	.source	__FILE__
