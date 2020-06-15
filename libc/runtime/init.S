/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/prot.h"
#include "libc/dce.h"
.yoink	__FILE__

/	Decentralized function for process initialization.
/
/	Modules may inject cheap data structure initialization code into
/	this function using the .init.start and .init.end macros. That
/	code can use the LODS and STOS instructions to initialize memory
/	that's restricted to read-only after initialization by PIRO.
/
/	This is fast, since the linker is able to roll-up initialization
/	for large codebases comprised of many modules, into a perfectly
/	linear order. It also enables a common pattern we use, which we
/	call “Referencing Is Initialization” (RII).
/
/	C/C++ code should favor using ordinary constructors, since under
/	normal circumstances the compiler will clobber RDI and RSI which
/	are granted special meanings within this function.
/
/	@param	r12 is argc (still callee saved)
/	@param	r13 is argv (still callee saved)
/	@param	r14 is envp (still callee saved)
/	@param	r15 is envp (still callee saved)
/	@note	rdi is __init_bss_start (callee monotonic lockstep)
/	@note	rsi is __init_rodata_start (callee monotonic lockstep)
/	@see	.init.start & .init.end (libc/macros.inc)
/	@see	ape/ape.lds
	.section .initprologue,"ax",@progbits
	.type	_init,@function
	.globl	_init
	.comm	g_runstate,4
_init:	push	%rbp
	mov	%rsp,%rbp
	.profilable
	ezlea	__init_bss_start,di
	ezlea	__init_rodata_start,si
	.previous/*
	...
	decentralized content
	...
	*/.section .initepilogue,"ax",@progbits
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
_woot:	leave
	ret
	.previous

/	Decentralized section for packed data structures & initializers.
/
/	@see	.initro (libc/macros.inc)
/	@see	ape/ape.lds
	.section .initroprologue,"a",@progbits
	.type	__init_rodata_start,@object
	.type	__init_rodata_end,@object
	.globl	__init_rodata_start,__init_rodata_end
	.hidden	__init_rodata_start,__init_rodata_end
	.align	__SIZEOF_POINTER__
__init_rodata_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .initroepilogue,"a",@progbits
__init_rodata_end:
	.byte	0x90
	.previous

/	Decentralized section for unpacked data structures.
/
/	Data in this section becomes read-only after initialization.
/
/	@see	.piro.bss.init (libc/macros.inc)
/	@see	libc/runtime/piro.c
/	@see	ape/ape.lds
	.section .piro.bss.init.1,"aw",@nobits
	.type	__init_bss_start,@object
	.type	__init_bss_end,@object
	.globl	__init_bss_start,__init_bss_end
	.hidden	__init_bss_start,__init_bss_end
	.align	__SIZEOF_POINTER__
__init_bss_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.bss.init.3,"aw",@nobits
__init_bss_end:
	.byte	0
	.previous

/	Special area for Windows NT support code.
/
/	Isolating this code adds value for Windows users by minimizing
/	page faults through improved locality. On System Five the PIRO
/	runtime can unmap these pages.
/
/	@see	libc/runtime/piro.c
/	@see	ape/ape.lds
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
