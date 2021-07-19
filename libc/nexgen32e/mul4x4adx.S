/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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

Mul4x4Adx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	mov	%rdx,%r12
	push	%rbx
	sub	$16,%rsp
	mov	(%rdx),%rdx
	mov	(%rsi),%rax
	mov	16(%rsi),%r11
	mov	24(%rsi),%r10
	xor	%r13d,%r13d
	mulx	%rax,%rbx,%rax
	mov	%rbx,-48(%rbp)
	mov	8(%rsi),%rbx
	mulx	%rbx,%rdx,%rcx
	adox	%rdx,%rax
	mov	(%r12),%rdx
	mulx	%r11,%rdx,%r9
	adox	%rdx,%rcx
	mov	(%r12),%rdx
	mulx	%r10,%rdx,%r8
	adox	%rdx,%r9
	adox	%r13,%r8
	xor	%r13d,%r13d
	mov	(%rsi),%r14
	mov	8(%r12),%rdx
	mulx	%r14,%r14,%r15
	adox	%r14,%rax
	adcx	%r15,%rcx
	mov	%rax,-56(%rbp)
	mulx	%rbx,%r14,%rax
	adox	%r14,%rcx
	adcx	%rax,%r9
	mulx	%r11,%r14,%rax
	adox	%r14,%r9
	adcx	%rax,%r8
	mulx	%r10,%rdx,%rax
	adox	%rdx,%r8
	mov	16(%r12),%rdx
	adcx	%r13,%rax
	adox	%r13,%rax
	mov	(%rsi),%r13
	xor	%r15d,%r15d
	mulx	%r13,%r13,%r14
	adox	%r13,%rcx
	adcx	%r14,%r9
	mulx	%rbx,%r14,%r13
	adox	%r14,%r9
	adcx	%r13,%r8
	mulx	%r11,%r14,%r13
	adox	%r14,%r8
	adcx	%r13,%rax
	mov	(%rsi),%rsi
	mulx	%r10,%rdx,%r13
	adox	%rdx,%rax
	adcx	%r15,%r13
	mov	24(%r12),%rdx
	adox	%r15,%r13
	mulx	%rsi,%r12,%rsi
	xor	%r14d,%r14d
	adox	%r12,%r9
	adcx	%rsi,%r8
	mulx	%rbx,%rsi,%rbx
	adox	%rsi,%r8
	adcx	%rbx,%rax
	mulx	%r11,%r11,%rsi
	mov	-56(%rbp),%rbx
	mov	%rcx,16(%rdi)
	adcx	%rsi,%r13
	mov	-48(%rbp),%rsi
	mov	%rbx,8(%rdi)
	adox	%r11,%rax
	mov	%r9,24(%rdi)
	mov	%r8,32(%rdi)
	mov	%rax,40(%rdi)
	mulx	%r10,%rdx,%r10
	adox	%rdx,%r13
	adcx	%r14,%r10
	mov	%r13,48(%rdi)
	adox	%r14,%r10
	mov	%rsi,(%rdi)
	mov	%r10,56(%rdi)
	add	$16,%rsp
	pop	%rbx
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	pop	%rbp
	ret
	.endfn	Mul4x4Adx,globl
