/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/notice.inc"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/dce.h"
#include "libc/runtime/mappings.h"
#include "libc/sysv/consts/prot.h"
.text.startup
.source	__FILE__

/	Cosmopolitan process entrypoint.
/
/	@param	r12 is argc
/	@param	r13 is argv
/	@param	r14 is environ
/	@param	r15 is auxv
/	@noreturn
__executive:
	.frame0
	ezlea	_base,bx

#ifdef __FAST_MATH__
	call	__fast_math
#endif
	call	_init
#if IsModeDbg()
	call	_init				# _init() is idempotent
#endif

/*
#if !IsTiny()
/	“Memory obfuscation for glibc, not for we”
/
/	64kb stack w/ 4kb guard alongside tuning in libc/integral/c.inc
/	e.g. -Werror=frame-larger-than=4096 is intended to guarantee no
/	stack overflow possible. We like malloc and only cleverly avoid
/	its use at the lowest levels of the runtime stack, without MMU.
/	We like this practicee because it's how Google runs production.
	mov	$kStackCeiling-STACKSIZE,%rdi
	mov	$STACKSIZE,%esi
	mov	$PROT_READ|PROT_WRITE,%edx
	mov	MAP_ANONYMOUS,%ecx
	or	MAP_FIXED,%ecx
	or	MAP_PRIVATE,%ecx
	mov	$-1,%r8d
	xor	%r9d,%r9d
	call	mmap
	cmp	$-1,%eax
	je	abort
	lea	STACKSIZE(%rax),%rsp
	xor	%ebp,%ebp
	mov	%rax,%rdi
	mov	$PAGESIZE,%esi
	mov	$PROT_NONE,%edx
	call	mprotect
	cmp	$-1,%eax
	je	abort
#endif
*/

	orl	$RUNSTATE_INITIALIZED,g_runstate(%rip)
	ezlea	__init_array_start,ax		# static ctors in forward order
	.weak	__init_array_start		# could be called multiple times
	ezlea	__init_array_end,cx		# idempotency recommended
	.weak	__init_array_end		# @see ape/ape.lds
1:	cmp	%rax,%rcx
	je	2f
	push	%rax
	push	%rcx
	call	*(%rax)
	pop	%rcx
	pop	%rax
	add	$8,%rax
	jmp	1b
2:	nop
#if !IsTrustworthy()
	mov	$PROT_READ,%edi
	call	__piro
#endif
	mov	%r12,%rdi
	mov	%r13,%rsi
	mov	%r14,%rdx
	.weak	main
	call	main
	mov	%eax,%edi
	call	exit
	.endfn	__executive,weak,hidden

#ifdef __PG__
/	Enables plaintext function tracing if --ftrace flag passed.
/
/	The --ftrace CLI arg is removed before main() is called. This
/	code is intended for diagnostic purposes and assumes binaries
/	are trustworthy and stack isn't corrupted. Logging plain text
/	allows program structure to easily be visualized and hotspots
/	identified w/ sed | sort | uniq -c | sort. A compressed trace
/	can be made by appending --ftrace 2>&1 | gzip -4 >trace.gz to
/	the CLI arguments. Have fun.
/
/	@see	libc/runtime/ftrace.greg.c
/	@see	libc/crt/crt.S
	.init.start 800,_init_ftrace
	push	%rdi
	push	%rsi
	xor	%edx,%edx
	loadstr	"--ftrace",di
	xor	%ecx,%ecx
0:	inc	%ecx
	mov	(%r13,%rcx,8),%rsi
	test	%edx,%edx
	jz	1f
	mov	%rsi,-8(%r13,%rcx,8)
1:	test	%rsi,%rsi
	jz	2f
	test	%edx,%edx
	jnz	0b
	call	tinystrcmp
	test	%eax,%eax
	setz	%dl
	jmp	0b
2:	sub	%rdx,%r12
	test	%edx,%edx
	jz	2f
	call	ftrace_init
2:	pop	%rsi
	pop	%rdi
	.init.end 800,_init_ftrace
#endif /* -pg */
