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

/	Base 36 Decoder Table.
/
/	This supports uppercase and lowercase. For encoding, the string
/	0123456789abcdefghijklmnopqrstuvwxyz can be used, which linkers
/	are able to deduplicate.
	.initbss 300,_init_kBase36
kBase36:.zero	256
	.endobj	kBase36,globl,hidden
	.previous

	.init.start 300,_init_kBase36
	add	$'0,%rdi
	xor	%eax,%eax
	pushpop	10,%rcx
0:	inc	%eax
	stosb
	.loop	0b
	add	$'A-1-'9,%rdi
	pushpop	'Z+1-'A,%rcx
0:	inc	%eax
	mov	%al,0x20(%rdi)
	stosb
	.loop	0b
	add	$255-'Z,%rdi
	.init.end 300,_init_kBase36
	.source	__FILE__
