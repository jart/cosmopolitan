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
#include "libc/macros.h"
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

//	System Five userspace program entrypoint.
//
//	@param	rsp is [n,argv₀..argvₙ₋₁,0,envp₀..,0,auxv₀..,0,..]
//	@note	FreeBSD is special (see freebsd/lib/csu/amd64/...)
//	@note	NetBSD will only zero the call-clobbered registers
//	@note	ape.S and ape-loader both set RCX to XNU on Darwin
//	@noreturn
_start:
	.cfi_startproc
#if defined(__x86_64__)
	.cfi_undefined rip
#elif defined(__aarch64__)
	.cfi_undefined x30
#endif /* __x86_64__ */

#if defined(__x86_64__)

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

#ifdef __FAST_MATH__
	push	%rax
	stmxcsr	(%rsp)
//
//	Enable hardware optimizations in violation of the IEEE standard.
//
//	- 0x0040 enables "DAZ: Denormals Are Zeros" in MXCSR. This causes the
//	  processor to turn denormal inputs into zero, before computing them.
//	  See Intel Manual Vol. 1 §10.2.3.4. It can be extremely impactful if
//	  your code encounters a lot of denormals at runtime, since denormals
//	  can make a flop take 20x or even 200x longer in the worst case with
//	  very cheap and/or very old microprocessors particularly with divide
//
//	- 0x8000 enables "FTZ: Flush To Zero" in MXCSR. This means a floating
//	  point operation that results in underflow will be set to zero, with
//	  the same sign, rather than producing a denormalized output. It will
//	  happen only if underflow trapping hasnt been enabled. See the Intel
//	  Manual Vol. 1 §10.2.3.3.
//
//	This code path is used in crtfastmath.o which is used by cosmocc when
//	a rube passes -ffast-math or -funsafe-math-optimizations at link time
//
	orl	$0x8040,(%rsp)
	ldmxcsr	(%rsp)
	pop	%rax
#endif

//	translates arguments from old stack abi
	mov	(%rsp),%ebx			// argc
	lea	8(%rsp),%rsi			// argv
	lea	16(%rsp,%rbx,8),%rdx		// envp
	mov	%rsp,__oldstack(%rip)
	mov	%rdx,__envp(%rip)

//	setup stack
	xor	%ebp,%ebp
	and	$-16,%rsp

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
	.section .yoink
	call	main
	.previous

////////////////////////////////////////////////////////////////////////////////
#elif defined(__aarch64__)

#if SupportsFreebsd()
//	save first arg
	mov	x4,x0
#endif

#ifdef __FAST_MATH__
//	Enable hardware optimizations in violation of the IEEE standard.
//	Bit 24 is FZ which means Flush denormals to Zero.
	mov	w0,#0x1000000
	msr	fpcr,x0
#endif

//	save original stack pointer
//	this is the first argument to cosmo() below
	mov	x0,sp

//	setup the stack
	mov	x28,#0
	mov	x29,#0
	mov	x30,#0
	and	sp,x0,#-16

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
	.section .yoink
	bl	main
	.previous

////////////////////////////////////////////////////////////////////////////////
#else
#error "architecture unsupported"
#endif /* __x86_64__ */
	.cfi_endproc
	.endfn	_start,weak,hidden
