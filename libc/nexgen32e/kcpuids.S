/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/lib/pc.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86feature.h"

/	Globally precomputed CPUID.
/
/	This module lets us check CPUID in 0.06ns rather than 51.00ns.
/	If every piece of native software linked this module, then the
/	world would be a much better place; since all the alternatives
/	are quite toilsome.
/
/	@see	www.felixcloutier.com/x86/cpuid
	.initbss 201,_init_kCpuids
kCpuids:.long	0,0,0,0				# EAX=0 (Basic Processor Info)
	.long	0,0,0,0				# EAX=1 (Processor Info)
	.long	0,0,0,0				# EAX=2
	.long	0,0,0,0				# EAX=7 (Extended Features)
	.long	0,0,0,0				# EAX=0x80000001 (NexGen32e)
	.long	0,0,0,0				# EAX=0x80000007 (APM)
	.long	0,0,0,0				# EAX=16h (CPU Frequency)
	.endobj	kCpuids,globl
	.previous

	.init.start 201,_init_kCpuids
	push	%rbx
	push	$0
	push	$0x16
	push	$0xffffffff80000007
	push	$0xffffffff80000001
	push	$7
	push	$2
	push	$1
	mov	%rdi,%r8
	xor	%eax,%eax
1:	xor	%ecx,%ecx
	cpuid
	stosl
	xchg	%eax,%ebx
	stosl
	xchg	%eax,%ecx
	stosl
	xchg	%eax,%edx
	stosl
2:	pop	%rax
	test	%eax,%eax			# EAX = stacklist->pop()
	jz	3f				# EAX ≠ 0 (EOL sentinel)
	cmp	KCPUIDS(0H,EAX)(%r8),%eax	# EAX ≤ CPUID.0 max leaf
	jle	1b				# CPUID too new to probe
	add	$4*4,%rdi
	jmp	2b
3:	nop
#if 0 && !X86_NEED(AVX2)
	testb	X86_HAVE(AVX)(%r8)
	jz	5f
	testb	X86_HAVE(OSXSAVE)(%r8)
	jz	4f
	xor	%ecx,%ecx
	xgetbv
	and	$XCR0_SSE|XCR0_AVX,%eax
	cmp	$XCR0_SSE|XCR0_AVX,%eax
	je	5f
4:	btr	$X86_BIT(AVX),X86_WORD(AVX)(%r8)
	btr	$X86_BIT(AVX2),X86_WORD(AVX2)(%r8)
#endif
5:	pop	%rbx
	.init.end 201,_init_kCpuids
	.source	__FILE__
