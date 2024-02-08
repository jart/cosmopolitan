/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "ape/relocations.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/runtime/pc.internal.h"
.real

//	Start the Cosmopolitan runtime after exiting UEFI Boot Services.
//
//	@param	rdi is mm
//	@param	rsi is new pml4t
//	@param	rdx is argc
//	@param	rcx is argv
//	@see	libc/runtime/efimain.greg.c
_EfiPostboot:
	cli
//	Define handy mnemonics for parameters & constants stored in
//	call-saved registers.
#define rMm   %r12
#define rArgc %r13
#define rArgv %r14
#define rBane %r15
	movabs	$BANE,rBane
	mov	%rdi,rMm
	mov	%rdx,rArgc
	lea	(rBane,%rcx),rArgv
	mov	$PHYSICAL(.Ltmpstk),%rax	// switch to temporary stack
	and	$-16,%al			// in physical space
	xchg	%rax,%rsp
	mov	$PHYSICAL(0f),%eax		// resume execution in copied
	jmp	*%rax				// image
0:	mov	$EFER,%ecx			// enable syscall/sysret & nx
	rdmsr
	or	$EFER_SCE|EFER_NXE,%eax
	wrmsr
	mov	%rsi,%cr3			// load new page table
	add	rBane,%rsp			// we can now switch stack to
	add	rBane,rMm			// negative address space
	mov	$1024*1024,%edx			// set up virtual memory
	mov	$1024*1024+_end,%ecx		// mapping
	sub	$__executable_start,%ecx
	call	__map_phdrs
	mov	$1f,%eax			// switch rip to virtual
	jmp	*%rax				// address space
1:	push	$0x037f
	fldcw	(%rsp)
	.weak	_gdtr
	lgdt	_gdtr				// switch to our own GDT
	mov	$GDT_LONG_DATA,%ax
	mov	%ax,%ds
	mov	%ax,%ss
	mov	%ax,%es
	mov	%ax,%fs
	mov	%ax,%gs
	.weak	ape_stack_vaddr
	.weak	ape_stack_memsz
	movabs	$ape_stack_vaddr,%rsp		// switch to final stack in
	add	$ape_stack_memsz,%rsp		// virtual address space
	movl	$0,0x7b000			// unmap null 2mb
	mov	rMm,%rdi
	xor	%esi,%esi			// free up now-unused pages
	xor	%edx,%edx
	call	__reclaim_boot_pages
	push	.Lenv0(%rip)			// envp[0][0]
	mov	%rsp,%rbp
	push	$0				// auxv[1][1]
	push	$0				// auxv[1][0]
	mov	(rArgv),%rax
	add	rBane,%rax
	push	%rax				// auxv[0][1]
	push	$31				// auxv[0][0] AT_EXECFN
	push	$0				// envp[1]
	push	%rbp				// envp[0]
	push	$0				// argv[argc] NULL
	lea	-8(rArgv,rArgc,8),%rsi		// push rest of argv, &
	mov	rArgc,%rcx			// adjust pointers to point to
	std					// negative space
2:	lodsq
	add	rBane,%rax
	push	%rax
	loop	2b
	cld
	push	rArgc				// argc
	pushpop	_HOSTMETAL,%rcx			// sets __hostos in crt.S
	xor	%ebp,%ebp
	xor	%eax,%eax
	xor	%ebx,%ebx
	xor	%edx,%edx
	xor	%edi,%edi
	xor	%esi,%esi
	xor	%r8d,%r8d
	xor	%r9d,%r9d
	xor	%r10d,%r10d
	xor	%r11d,%r11d
	xor	%r12d,%r12d
	xor	%r13d,%r13d
	xor	%r14d,%r14d
	xor	%r15d,%r15d
	push	$GDT_LONG_CODE
	.weak	_start
	push	$_start
	lretq
	.endfn	_EfiPostboot,globl,hidden
	.rodata
.Lenv0:	.asciz	"METAL=1"
	.bss
	.space	0x1000
.Ltmpstk:
	.previous

	.weak	__executable_start
	.weak	_end
