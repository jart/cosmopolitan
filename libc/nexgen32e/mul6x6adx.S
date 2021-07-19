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

Mul6x6Adx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbx
	mov	%rdx,%rbx
	sub	$24,%rsp
	mov	(%rdx),%rdx
	xor	%r8d,%r8d
	mulx	(%rsi),%rcx,%rax
	mulx	8(%rsi),%rdx,%r12
	mov	%rcx,-48(%rbp)
	adox	%rdx,%rax
	mov	(%rbx),%rdx
	mulx	16(%rsi),%rdx,%r15
	adox	%rdx,%r12
	mov	(%rbx),%rdx
	mulx	24(%rsi),%rdx,%r10
	adox	%rdx,%r15
	mov	(%rbx),%rdx
	mulx	32(%rsi),%rdx,%r9
	adox	%rdx,%r10
	mov	(%rbx),%rdx
	mulx	40(%rsi),%rdx,%rcx
	adox	%rdx,%r9
	mov	8(%rbx),%rdx
	adox	%r8,%rcx
	mulx	(%rsi),%r13,%r11
	xor	%r8d,%r8d
	adox	%r13,%rax
	adcx	%r11,%r12
	mov	%rax,-56(%rbp)
	mulx	8(%rsi),%r11,%rax
	adox	%r11,%r12
	adcx	%rax,%r15
	mov	%r12,%r14
	mulx	16(%rsi),%r11,%rax
	adox	%r11,%r15
	adcx	%rax,%r10
	mulx	24(%rsi),%r11,%rax
	adox	%r11,%r10
	adcx	%rax,%r9
	mulx	32(%rsi),%r11,%rax
	adox	%r11,%r9
	adcx	%rax,%rcx
	mulx	40(%rsi),%rdx,%rax
	adox	%rdx,%rcx
	adcx	%r8,%rax
	mov	16(%rbx),%rdx
	adox	%r8,%rax
	mulx	(%rsi),%r13,%r8
	xor	%r11d,%r11d
	adox	%r13,%r14
	mov	%r14,-64(%rbp)
	adcx	%r8,%r15
	mulx	8(%rsi),%r12,%r8
	adox	%r12,%r15
	adcx	%r8,%r10
	mulx	16(%rsi),%r12,%r8
	adox	%r12,%r10
	adcx	%r8,%r9
	mulx	24(%rsi),%r12,%r8
	adox	%r12,%r9
	adcx	%r8,%rcx
	mulx	32(%rsi),%r12,%r8
	adox	%r12,%rcx
	adcx	%r8,%rax
	mulx	40(%rsi),%rdx,%r8
	adox	%rdx,%rax
	adcx	%r11,%r8
	mov	24(%rbx),%rdx
	adox	%r11,%r8
	mulx	(%rsi),%r13,%r11
	xor	%r12d,%r12d
	adox	%r13,%r15
	adcx	%r11,%r10
	mulx	8(%rsi),%r13,%r11
	adox	%r13,%r10
	adcx	%r11,%r9
	mulx	16(%rsi),%r13,%r11
	adox	%r13,%r9
	adcx	%r11,%rcx
	mulx	24(%rsi),%r13,%r11
	adox	%r13,%rcx
	adcx	%r11,%rax
	mulx	32(%rsi),%r13,%r11
	adox	%r13,%rax
	adcx	%r11,%r8
	mulx	40(%rsi),%rdx,%r11
	adox	%rdx,%r8
	mov	32(%rbx),%rdx
	adcx	%r12,%r11
	mulx	(%rsi),%r14,%r13
	adox	%r12,%r11
	xor	%r12d,%r12d
	adox	%r14,%r10
	adcx	%r13,%r9
	mulx	8(%rsi),%r14,%r13
	adox	%r14,%r9
	adcx	%r13,%rcx
	mulx	16(%rsi),%r14,%r13
	adox	%r14,%rcx
	adcx	%r13,%rax
	mulx	24(%rsi),%r14,%r13
	adox	%r14,%rax
	adcx	%r13,%r8
	mulx	32(%rsi),%r14,%r13
	adox	%r14,%r8
	adcx	%r13,%r11
	mulx	40(%rsi),%rdx,%r13
	adox	%rdx,%r11
	adcx	%r12,%r13
	mov	40(%rbx),%rdx
	adox	%r12,%r13
	mulx	(%rsi),%r14,%rbx
	xor	%r12d,%r12d
	adox	%r14,%r9
	adcx	%rbx,%rcx
	mulx	8(%rsi),%r14,%rbx
	adox	%r14,%rcx
	adcx	%rbx,%rax
	mulx	16(%rsi),%r14,%rbx
	adox	%r14,%rax
	adcx	%rbx,%r8
	mulx	24(%rsi),%r14,%rbx
	adox	%r14,%r8
	adcx	%rbx,%r11
	mulx	32(%rsi),%r14,%rbx
	mulx	40(%rsi),%rsi,%rdx
	adox	%r14,%r11
	adcx	%rbx,%r13
	adox	%rsi,%r13
	adcx	%r12,%rdx
	adox	%r12,%rdx
	mov	-48(%rbp),%rsi
	mov	-56(%rbp),%rbx
	mov	%r15,24(%rdi)
	mov	-64(%rbp),%r14
	mov	%r13,80(%rdi)
	mov	%rbx,8(%rdi)
	mov	%r14,16(%rdi)
	mov	%rsi,(%rdi)
	mov	%r10,32(%rdi)
	mov	%r9,40(%rdi)
	mov	%rcx,48(%rdi)
	mov	%rax,56(%rdi)
	mov	%r8,64(%rdi)
	mov	%r11,72(%rdi)
	mov	%rdx,88(%rdi)
	add	$24,%rsp
	pop	%rbx
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	pop	%rbp
	ret
	.endfn	Mul6x6Adx,globl
