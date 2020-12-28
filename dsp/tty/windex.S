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
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"
.source	__FILE__

/	Dispatches to fastest windex() implementation.
	.initbss 300,_init_windex
windex:	.quad	0
	.endobj	windex,globl
	.previous

	.init.start 300,_init_windex
	ezlea	windex$avx2,ax
#if !X86_NEED(AVX2)
	ezlea	windex$sse4,dx
	testb	X86_HAVE(AVX2)+kCpuids(%rip)
	cmovz	%rdx,%rax
#endif /* AVX */
#if !X86_NEED(SSE4_2)
	ezlea	windex$k8,dx
	testb	X86_HAVE(SSE4_2)+kCpuids(%rip)
	cmovz	%rdx,%rax
#endif /* SSE4 */
	stosq
	.init.end 300,_init_windex
