/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
//	@param	rdx is stack pointer (becomes zero)
//	@param	rcx is passed through as-is
//	@noreturn
Launch:	xor	%r8d,%r8d
	xor	%r9d,%r9d
	xor	%r10d,%r10d
	xor	%r11d,%r11d
	xor	%r12d,%r12d
	xor	%r13d,%r13d
	xor	%r14d,%r14d
	xor	%r15d,%r15d
	mov	%rdx,%rsp
	xor	%edx,%edx
	push	%rsi
	xor	%esi,%esi
	xor	%ebp,%ebp
	xor	%ebx,%ebx
	xor	%eax,%eax
	ret
	.endfn	Launch,globl
