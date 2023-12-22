/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

//	Calls _start() function of loaded program.
//
//	When the program entrypoint is called, all registers shall be
//	cleared, with the exception of (1) %rdi will be equal to %rsp
//	on FreeBSD and (2) %cl will contain the detected host OS code
//
//	We clear all the general registers we can to have some wiggle
//	room, to extend the behavior of this loader in the future. We
//	don't need to clear the XMM registers because your APE loader
//	should be compiled using gcc/clang's -mgeneral-regs-only flag
//
//	@param	rdi is passed through as-is
//	@param	rsi is address of entrypoint (becomes zero)
//	@param	rdx is passed through as-is
//	@param	rcx is passed through as-is
//	@param	r8  is stack pointer (becomes zero)
//	@noreturn
Launch:
#ifdef __aarch64__

	mov	x16,x1
	mov	sp,x4
	mov	x1,0
	mov	x4,0
	mov	x5,0
	mov	x6,0
	mov	x7,0
	mov	x8,0
	mov	x9,0
	mov	x10,0
	mov	x11,0
	mov	x12,0
	mov	x13,0
	mov	x14,0
	mov	x15,0
	mov	x17,0
	mov	x19,0
	mov	x20,0
	mov	x21,0
	mov	x22,0
	mov	x23,0
	mov	x24,0
	mov	x25,0
	mov	x26,0
	mov	x27,0
	mov	x28,0
	mov	x29,0
	mov	x30,0
	br	x16

#else

	mov	%r8,%rsp
	xor	%r8d,%r8d
	xor	%r9d,%r9d
	xor	%r10d,%r10d
	xor	%r11d,%r11d
	xor	%r12d,%r12d
	xor	%r13d,%r13d
	xor	%r14d,%r14d
	xor	%r15d,%r15d
	push	%rsi
	xor	%esi,%esi
	xor	%ebp,%ebp
	xor	%ebx,%ebx
	xor	%eax,%eax
	ret

#endif
	.endfn	Launch,globl
