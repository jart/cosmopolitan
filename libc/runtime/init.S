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
#include "libc/sysv/consts/prot.h"
#include "libc/dce.h"

//	Decentralized function for process initialization.
//
//	Modules may inject cheap data structure initialization code into
//	this function using the .init.start and .init.end macros. That
//	code can use the LODS and STOS instructions to initialize memory
//	that's restricted to read-only after initialization by PIRO.
//
//	This is fast, since the linker is able to roll-up initialization
//	for large codebases comprised of many modules, into a perfectly
//	linear order. It also enables a common pattern we use, which we
//	call “Referencing Is Initialization” (RII).
//
//	C/C++ code should favor using ordinary constructors, since under
//	normal circumstances the compiler will clobber RDI and RSI which
//	are granted special meanings within this function.
//
//	@param	r12 is argc (still callee saved)
//	@param	r13 is argv (still callee saved)
//	@param	r14 is envp (still callee saved)
//	@param	r15 is envp (still callee saved)
//	@note	rdi is __init_bss_start (callee monotonic lockstep)
//	@note	rsi is __init_rodata_start (callee monotonic lockstep)
//	@see	.init.start & .init.end (libc/macros.internal.h)
//	@see	ape/ape.lds
	.section .initprologue,"ax",@progbits
	.type	_init,@function
	.globl	_init
_init:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	ezlea	__init_bss_start,di
	ezlea	__init_rodata_start,si
#elif defined(__aarch64__)
	stp	x29,x30,[sp,-16]!
	mov	x29,sp
#endif
	.previous/*
	...
	decentralized content
	...
	*/.section .initepilogue,"ax",@progbits
#ifdef __x86_64__
#if IsModeDbg()
_init_check_rdi_rsi:
	jmp	2f
1:	call	abort
2:	ezlea	__init_bss_end,ax
	cmp	%rax,%rdi
	jne	1b
	ezlea	__init_rodata_end,ax
	cmp	%rax,%rsi
	jne	1b
3:	.endfn	_init_check_rdi_rsi
#endif
	leave
#elif defined(__aarch64__)
	ldp	x29,x30,[sp],#16
#endif
	ret
	.previous

#ifdef __x86_64__

//	Decentralized section for packed data structures & initializers.
//
//	@see	.initro (libc/macros.internal.h)
//	@see	ape/ape.lds
	.section .initroprologue,"a",@progbits
	.type	__init_rodata_start,@object
	.type	__init_rodata_end,@object
	.globl	__init_rodata_start,__init_rodata_end
	.hidden	__init_rodata_start,__init_rodata_end
	.balign	__SIZEOF_POINTER__
	.underrun
__init_rodata_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .initroepilogue,"a",@progbits
__init_rodata_end:
	.byte	0x90
	.overrun
	.previous

//	Decentralized section for unpacked data structures.
//
//	Data in this section becomes read-only after initialization.
//
//	@see	.piro.bss.init (libc/macros.internal.h)
//	@see	libc/runtime/piro.c
//	@see	ape/ape.lds
	.section .piro.bss.init.1,"aw",@nobits
	.type	__init_bss_start,@object
	.type	__init_bss_end,@object
	.globl	__init_bss_start,__init_bss_end
	.hidden	__init_bss_start,__init_bss_end
	.balign	__SIZEOF_POINTER__
	.underrun
__init_bss_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.bss.init.3,"aw",@nobits
__init_bss_end:
	.byte	0
	.overrun
	.previous

//	Special area for Windows NT support code.
//
//	Isolating this code adds value for Windows users by minimizing
//	page faults through improved locality. On System Five the PIRO
//	runtime can unmap these pages.
//
//	@see	libc/runtime/piro.c
//	@see	ape/ape.lds
	.section .textwindowsprologue,"ax",@progbits
	.type	__text_windows_start,@object
	.type	__text_windows_end,@object
	.globl	__text_windows_start,__text_windows_end
	.hidden	__text_windows_start,__text_windows_end
	int3
__text_windows_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .textwindowsepilogue,"ax",@progbits
__text_windows_end:
	int3
	.previous

#endif /* __x86_64__ */
