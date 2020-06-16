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
