/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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

//	Code morphing TLS polyfills for The New Technology.
//
//	@note	msvc generates this code so it's stable
//	@note	func ordering follows x86 reg encoding
//	@note	each function is exactly 18 bytes
//	@see	__enable_threads()

__tls_mov_nt_rax:
	push	%rcx
	mov	__tls_index(%rip),%ecx
	mov	%gs:0x1480(,%rcx,8),%rax
	pop	%rcx
	ret
	.endfn	__tls_mov_nt_rax,globl,hidden

__tls_mov_nt_rcx:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rcx
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rcx

__tls_mov_nt_rdx:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rdx
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rdx

__tls_mov_nt_rbx:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rbx
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rbx

__tls_mov_nt_rsp:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rsp
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rsp

__tls_mov_nt_rbp:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rbp
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rbp

__tls_mov_nt_rsi:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rsi
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rsi

__tls_mov_nt_rdi:
	push	%rax
	mov	__tls_index(%rip),%eax
	mov	%gs:0x1480(,%rax,8),%rdi
	pop	%rax
	ret
	.endfn	__tls_mov_nt_rdi

////////////////////////////////////////////////////////////////////////////////

__tls_add_nt_rax:
	push	%rcx
	mov	__tls_index(%rip),%ecx
	add	%gs:0x1480(,%rcx,8),%rax
	pop	%rcx
	ret
	.endfn	__tls_add_nt_rax,globl,hidden

__tls_add_nt_rcx:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rcx
	pop	%rax
	ret
	.endfn	__tls_add_nt_rcx

__tls_add_nt_rdx:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rdx
	pop	%rax
	ret
	.endfn	__tls_add_nt_rdx

__tls_add_nt_rbx:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rbx
	pop	%rax
	ret
	.endfn	__tls_add_nt_rbx

__tls_add_nt_rsp:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rsp
	pop	%rax
	ret
	.endfn	__tls_add_nt_rsp

__tls_add_nt_rbp:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rbp
	pop	%rax
	ret
	.endfn	__tls_add_nt_rbp

__tls_add_nt_rsi:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rsi
	pop	%rax
	ret
	.endfn	__tls_add_nt_rsi

__tls_add_nt_rdi:
	push	%rax
	mov	__tls_index(%rip),%eax
	add	%gs:0x1480(,%rax,8),%rdi
	pop	%rax
	ret
	.endfn	__tls_add_nt_rdi
