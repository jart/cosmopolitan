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
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/map.h"
#include "libc/dce.h"
.text.startup
.source	__FILE__

/	Stack frame that owns process from spawn to exit.
/
/	@param	edi is argc
/	@param	rsi is argv
/	@param	rdx is environ
/	@param	rcx is auxv
/	@noreturn
_executive:
	push	%rbp
	mov	%rsp,%rbp
	ezlea	_base,bx
	mov	%edi,%r12d
	mov	%rsi,%r13
	mov	%rdx,%r14
	mov	%rcx,%r15
	call	_spawn
	call	_getstack
	mov	%rax,%rdi
	.weak	main
	mov	$main,%esi
	mov	%r12,%rdx
	mov	%r13,%rcx
	mov	%r14,%r8
	mov	%r15,%r9
	call	_setstack
	mov	%eax,%edi
	call	exit
	.endfn	_executive,weak,hidden
	ud2

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
