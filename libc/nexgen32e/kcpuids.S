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
#include "libc/runtime/pc.internal.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86feature.h"

	.initbss 201,_init_kCpuids
//	Globally precomputed CPUID.
//
//	This module lets us check CPUID in 0.06ns rather than 51.00ns.
//	If every piece of native software linked this module, then the
//	world would be a much better place; since all the alternatives
//	are quite toilsome.
//
//	@see	www.felixcloutier.com/x86/cpuid
kCpuids:.long	0,0,0,0				// EAX=0 (Basic Processor Info)
	.long	0,0,0,0				// EAX=1 (Processor Info)
	.long	0,0,0,0				// EAX=2
	.long	0,0,0,0				// EAX=7 (Extended Features)
	.long	0,0,0,0				// EAX=0x80000001 (NexGen32e)
	.long	0,0,0,0				// EAX=0x80000007 (APM)
	.long	0,0,0,0				// EAX=16h (CPU Frequency)
	.long	0,0,0,0				// EAX=7 ECX=1 (Extended Feats)
	.endobj	kCpuids,globl
	.previous

	.init.start 201,_init_kCpuids
	push	%rbx
	push	$0
	push	$1
	push	$7
	push	$0
	push	$0x16
	push	$0
	push	$0xffffffff80000007
	push	$0
	push	$0xffffffff80000001
	push	$0
	push	$7
	push	$0
	push	$2
	push	$0
	push	$1
	mov	%rdi,%r8
	xor	%eax,%eax
	xor	%ecx,%ecx
1:	nop
#ifdef FEATURELESS
//	It's been reported that GDB reverse debugging doesn't
//	understand VEX encoding. The workaround is to put:
//
//	    CPPFLAGS = -DFEATURELESS
//
//	Inside your ~/.cosmo.mk file.
	xor	%eax,%eax
	xor	%ebx,%ebx
	xor	%ecx,%ecx
	xor	%edx,%edx
#else
	cpuid
#endif
	stosl
	xchg	%eax,%ebx
	stosl
	xchg	%eax,%ecx
	stosl
	xchg	%eax,%edx
	stosl
2:	pop	%rax
	test	%eax,%eax			// EAX = stacklist->pop()
	jz	3f				// EAX ≠ 0 (EOL sentinel)
	pop	%rcx				// HERE WE GO AGAIN CPUID
	cmp	KCPUIDS(0H,EAX)(%r8),%al	// EAX ≤ CPUID.0 max leaf
	jbe	1b				// CPUID too new to probe
	add	$4*4,%rdi
	jmp	2b
3:	nop

//	test if cpu supports avx
	testb	X86_HAVE(AVX)(%r8)
	jz	7f
	testb	X86_HAVE(OSXSAVE)(%r8)
	jz	5f
	xor	%ecx,%ecx
	xgetbv
	mov	%eax,%ecx

//	test if operating system saves avx registers
	and	$XCR0_SSE|XCR0_AVX,%eax
	cmp	$XCR0_SSE|XCR0_AVX,%eax
	jne	5f

//	test if operating system saves avx512 registers
	and	$XCR0_OPMASK|XCR0_ZMM_HI256|XCR0_HI16_ZMM,%ecx
	cmp	$XCR0_OPMASK|XCR0_ZMM_HI256|XCR0_HI16_ZMM,%ecx
	jne	6f
	je	7f

//	operating system doesn't support avx
5:	btr	$X86_BIT(AVX),X86_WORD(AVX)(%r8)
	btr	$X86_BIT(AVX2),X86_WORD(AVX2)(%r8)

//	operating system supports avx but not avx512
6:	andl	$~(1<<30|1<<28|1<<17|1<<27|1<<16|1<<21|1<<26|1<<31),KCPUIDS(7H, EBX)(%r8)
	andl	$~(1<<1|1<<12|1<<6|1<<11|1<<14),KCPUIDS(7H, ECX)(%r8)
	andl	$~(1<<2|1<<3|1<<8),KCPUIDS(7H, EDX)(%r8)
	andl	$~(1<<5),KCPUIDS(7H_1H, EAX)(%r8)

//	we're done
7:	pop	%rbx
	.init.end 201,_init_kCpuids
