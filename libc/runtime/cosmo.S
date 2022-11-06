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
#include "libc/macros.internal.h"
#include "libc/notice.inc"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/map.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/dce.h"
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

#ifdef SYSDEBUG
	call	__strace_init
	mov	%eax,%r12d
#endif /* SYSDEBUG */

//	Windows always initializes FPU to douuble precision.
//	WSL breaks Linux ABI by initializing FPU to double precision.
//	This code makes long double long again.
//
//	@see System V Application Binary Interface NexGen32e Architecture
//	     Processor Supplement, Version 1.0, December 5th, 2018
//	     Section 3.4.1: Initial Stack and Register State
	fldcw	1f(%rip)
	.rodata
	.align	2
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
	ezlea	__init_array_start,ax		# static ctors in forward order
	.weak	__init_array_start		# could be called multiple times
	ezlea	__init_array_end,cx		# idempotency recommended
	.weak	__init_array_end		# @see ape/ape.lds
1:	cmp	%rax,%rcx
	je	2f
	push	%rax
	push	%rcx
	call	.Largs
	call	*(%rax)
	pop	%rcx
	pop	%rax
	add	$8,%rax
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

#if !IsTiny()
//	Creates deterministically addressed stack we can use
//
//	This helps debugging be more comprehensible, because
//	when diagnosing low-level problems when error report
//	isn't working, sometimes numbers are all you have to
//	go on, and we can't use them if kernel hardening has
//	configured that meaningful data to be randomized.
//
//	Having deterministic addresses is also key to ensure
//	builds, execution, and other things are reproducible
	.init.start 304,_init_stack
	testb	IsWindows()
	jnz	9f
	testb	IsMetal()
	jnz	9f
	push	%rdi
	push	%rsi

//	allocate stack
	movabs	$ape_stack_vaddr,%rdi
	mov	$ape_stack_memsz,%esi
	mov	$ape_stack_prot,%edx
	mov	$MAP_STACK,%ecx
	or	MAP_ANONYMOUS,%ecx
	or	$-1,%r8
	xor	%r9d,%r9d
	push	%rsi
	push	%rsi
	call	mmap
	pop	%r8
	pop	%r8
	pop	%rsi
	pop	%rdi
	cmp	$-1,%rax
	je	9f

//	switch stacks
//
//	we subtract 8 because the openbsd kernel always checks rsp
//	is on a MAP_STACK interval non-inclusively of stack + size
	leave
	pop	%rcx			# return address
	sub	$8,%r8d			# openbsd:stackbound
	lea	(%rax,%r8),%rsp
	mov	$ape_stack_align,%eax
	neg	%rax
	and	%rax,%rsp
	push	%rcx
	push	%rbp
	mov	%rsp,%rbp

9:	nop
	.init.end 304,_init_stack
	.weak	ape_stack_prot
	.weak	ape_stack_vaddr
	.weak	ape_stack_memsz
	.weak	ape_stack_align
#endif

#if IsAsan()
	.init.start 305,_init_symbols
	push	%rdi
	push	%rsi
	call	__init_symbols
	pop	%rsi
	pop	%rdi
	.init.end 305,_init_symbols
#endif

#ifdef __PG__
	.init.start 306,_init_ftrace
	push	%rdi
	push	%rsi
	call	ftrace_init
	mov	%eax,%r12d
	pop	%rsi
	pop	%rdi
	.init.end 306,_init_ftrace
#endif

#if IsModeDbg()
#ifdef SYSDEBUG
	.init.start 307,_init_printargs
	cmpl	$0,__strace(%rip)
	jz	1f
	push	%rdi
	push	%rsi
	loadstr	STRACE_PROLOGUE,di
	call	__printargs
	pop	%rsi
	pop	%rdi
1:	.init.end 307,_init_printargs
#endif
#endif
