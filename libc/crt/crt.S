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
#include "libc/notice.inc"
.section .start,"ax",@progbits

#if SupportsXnu() && defined(__x86_64__)
//	XNU AMD64 Entrypoint
//
//	@note	Rosetta barely implements MAC_LC_UNIXTHREAD
//	@note	Rosetta sets many registers to weird values
_apple:	mov	$_HOSTXNU,%cl
	jmp	1f
	.endfn	_apple,weak,hidden
#endif

#if SupportsWindows() && defined(__x86_64__) && !IsTiny()
//	implements all win32 apis on non-windows hosts
//	it is just enough code to get a good backtrace
__oops_win32:
	ud2
	nop
	.endfn	__oops_win32
#endif

//	System Five userspace program entrypoint.
//
//	@param	rsp is [n,argv₀..argvₙ₋₁,0,envp₀..,0,auxv₀..,0,..]
//	@note	FreeBSD is special (see freebsd/lib/csu/amd64/...)
//	@note	NetBSD will only zero the call-clobbered registers
//	@note	ape.S and ape-loader both set RCX to XNU on Darwin
//	@noreturn
_start:
#ifdef __x86_64__

#if SupportsFreebsd()
//	detect free besiyata dishmaya
	test	%rdi,%rdi
	cmovnz	%rdi,%rsp
	jz	0f
	movb	$_HOSTFREEBSD,%cl
0:
#endif

//	set operating system when already detected
1:	mov	%cl,__hostos(%rip)

	mov	%rdx,__program_executable_name(%rip)

//	get startup timestamp as early as possible
//	its used by --strace flag and kprintf() %T
	rdtsc
	ezlea	kStartTsc,bx
	mov	%eax,(%rbx)
	mov	%edx,4(%rbx)

//	translates arguments from old stack abi
	mov	(%rsp),%ebx			// argc
	lea	8(%rsp),%rsi			// argv
	lea	16(%rsp,%rbx,8),%rdx		// envp
	mov	%rsp,__oldstack(%rip)
	mov	%rdx,__envp(%rip)

//	setup stack
	xor	%ebp,%ebp
	and	$ape_stack_round,%rsp

#if SupportsWindows() && !IsTiny()
//	make win32 imps crash
	.weak	ape_idata_iat
	.weak	ape_idata_iatend
	ezlea	__oops_win32,ax
	ezlea	ape_idata_iat,di
	ezlea	ape_idata_iatend,cx
	sub	%rdi,%rcx
	shr	$3,%ecx
	rep stosq
#endif

//	scan through environment varis
//	find start of auxiliary values
	xor	%eax,%eax
	or	$-1,%ecx
	mov	%rdx,%rdi
	repnz scasq
	mov	%rdi,%rcx			// auxv

#if SupportsXnu()
//	xnu doesn't have auxiliary values
	testb	IsXnu()
	jz	1f				// polyfill xnu auxv
	push	$0				// auxv[1][1]=0
	push	$0				// auxv[1][0]=0
	mov	%rsp,%rcx			// auxv
#endif

//	enter cosmopolitan runtime
1:	mov	%ebx,%edi
	call	cosmo
9:	.unreachable

//	strongly link main() function (discarded by linker)
//	libc_runtime had to weakly link, due to package.com
	.section .yoink
	call	main
	.previous

////////////////////////////////////////////////////////////////////////////////
#elif defined(__aarch64__)

#if SupportsFreebsd()
//	save first arg
	mov	x4,x0
#endif

//	save original stack pointer
//	this is the first argument to cosmo() below
	mov	x0,sp

//	setup the stack
	mov	x28,#0
	mov	x29,#0
	mov	x30,#0
	ldr	x1,=ape_stack_round
	and	x1,x0,x1
	mov	sp,x1

//	second arg shall be struct Syslib passed by ape-m1.c
//	used to talk to apple's authoritarian libraries
//	should be set to zero on other platforms
	mov	x1,x15

//	third arg is program path passed by loader
//	fourth arg is detected os passed by loader

//	switch to c code
	bl	cosmo
	.unreachable

//	strongly link main() function (discarded by linker)
//	libc_runtime had to weakly link, due to package.com
	.section .yoink
	bl	main
	.previous

////////////////////////////////////////////////////////////////////////////////
#else
#error "architecture unsupported"
#endif /* __x86_64__ */
	.endfn	_start,weak,hidden
