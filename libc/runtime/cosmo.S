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
#include "libc/macros.internal.h"
#include "libc/notice.inc"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/map.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/dce.h"
#ifdef __x86_64__
.text.startup

//	Cosmopolitan runtime.
//
//	@param	edi is argc
//	@param	rsi is argv
//	@param	rdx is environ
//	@param	rcx is auxv
//	@noreturn
cosmo:	push	%rbp
	mov	%rsp,%rbp
	mov	%edi,%r12d
	mov	%rsi,%r13
	mov	%rdx,%r14
	mov	%rcx,%r15

#if SYSDEBUG
	call	__strace_init
	mov	%eax,%r12d
#endif /* SYSDEBUG */

#ifndef NOX87
//	Windows always initializes FPU to douuble precision.
//	WSL breaks Linux ABI by initializing FPU to double precision.
//	This code makes long double long again.
//
//	@see System V Application Binary Interface NexGen32e Architecture
//	     Processor Supplement, Version 1.0, December 5th, 2018
//	     Section 3.4.1: Initial Stack and Register State
	fldcw	1f(%rip)
	.rodata
	.balign	2
//	8087 FPU Control Word
//	 IM: Invalid Operation ───────────────┐
//	 DM: Denormal Operand ───────────────┐│
//	 ZM: Zero Divide ───────────────────┐││
//	 OM: Overflow ─────────────────────┐│││
//	 UM: Underflow ───────────────────┐││││
//	 PM: Precision ──────────────────┐│││││
//	 PC: Precision Control ───────┐  ││││││
//	  {float,∅,double,long double}│  ││││││
//	 RC: Rounding Control ──────┐ │  ││││││
//	  {even, →-∞, →+∞, →0}      │┌┤  ││││││
//	                           ┌┤││  ││││││
//	                          d││││rr││││││
1:	.short	0b00000000000000000001101111111
	.previous
#endif

#ifdef __FAST_MATH__
	push	%rax
	stmxcsr	(%rsp)
	orl	$0x8040,(%rsp)
	ldmxcsr	(%rsp)
	pop	%rax
#endif

//	run assembly init
	call	_init

//	call constructors
	ezlea	__init_array_end,ax		// static ctors in forward order
	.weak	__init_array_end		// could be called multiple times
	ezlea	__init_array_start,cx		// idempotency recommended
	.weak	__init_array_start		// @see ape/ape.lds
1:	cmp	%rax,%rcx
	je	2f
	sub	$8,%rax
	push	%rax
	push	%rcx
	call	.Largs
	call	*(%rax)
	pop	%rcx
	pop	%rax
	jmp	1b

//	call main()
2:	call	.Largs
	.weak	main
	call	main
	xchg	%eax,%edi
	call	exit

.Largs:	mov	%r12d,%edi
	mov	%r13,%rsi
	mov	%r14,%rdx
	mov	%r15,%rcx
	ret
	.endfn	cosmo,weak

//	Enables Thread Local Storage.
	.init.start 304,_init_tls
	push	%rdi
	push	%rsi
	call	__enable_tls
	pop	%rsi
	pop	%rdi
	.init.end 304,_init_tls

#if IsAsan()
	.init.start 305,_init_symbols
	push	%rdi
	push	%rsi
	call	__init_symbols
	pop	%rsi
	pop	%rdi
	.init.end 305,_init_symbols
#endif

#ifdef FTRACE
	.init.start 306,_init_ftrace
	push	%rdi
	push	%rsi
	call	ftrace_init
	mov	%eax,%r12d
	pop	%rsi
	pop	%rdi
	.init.end 306,_init_ftrace
#endif

#endif /* __x86_64__ */
