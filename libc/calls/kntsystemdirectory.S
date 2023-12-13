/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/dce.h"
#include "libc/macros.internal.h"

#define BYTES 64

//	RII constant holding 'C:/WINDOWS/SYSTEM32' directory.
//
//	@note	guarantees trailing slash if non-empty
	.initbss 300,_init_kNtSystemDirectory
kNtSystemDirectory:
	.zero	BYTES
	.endobj	kNtSystemDirectory,globl
	.previous

	.init.start 300,_init_kNtSystemDirectory
#if SupportsWindows()
	testb	IsWindows()
	jz	1f
	pushpop	BYTES,%rdx
	mov	__imp_GetSystemDirectoryA(%rip),%rax
	call	__getntsyspath
	jmp	2f
#endif
1:	add	$BYTES,%rdi
2:	.init.end 300,_init_kNtSystemDirectory
