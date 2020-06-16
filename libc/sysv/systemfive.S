/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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
#include "libc/dce.h"
#include "libc/macros.h"
.source	__FILE__

/*                                           ▄▄▄
                       ▄▄▄                    ▀▓▓▒▄
                     ▄▓▒▒░                      ▀▓▒▒▒▄
                   ▄▓▓▓▒▀              ▄▄▄▄      ▒▓▒▒░▒▄
                  ▄▓▓▓▒▓        ▄▄▓██▓▓▓▓▒▒▒▒▓▓▄▄▓▓▒▒▒░░▒
                  ▓▓▓▓▒▒▒▄▄  ░▒█▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓▒░░▒░
                  ██▓▓▓▒▒░░▒▒▒▒▓▓▓▓▓▓▒▓▒░▒▒░▀▒▒▒▒░▀░▒▒▒░▒
                  ▓▓▓▓▓▓▓▒▒▒▒▒▒▓▓▒▓▓▒▒▒░▒▒░░  ░▒▒░  ░▒▒▒▒
                   ▀▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒░░▒░░    ░▒▒  ░ ▀▒▒
                     ▀▓▓█▓▓▓▓▓▓▓▓▓▓▒▒░░▒▒░░   ░░░▓░ ▓░░░▒
                       ▀▀█▓███▓▓▓▓▓▒▒░░░▒░░  ░█▓░█▓░█▓▓▄▒░
                          ░▓██▓▓▓▓▓▒▒░░░▒░░  ░████▓▒▓█▓▀░▀▄
                          ░▓██▓▓▓▓▓▒▒▒░░░▒░░  ▒██▓▒▒▒▒▒▒░░░▒
                           ████▓▓▓▓▓▒▒▒▒▒▒▒▒▒░░▒▓▓▒░░░░▒░░░▒░ ░░░░░
                           ░▓███▓▓▓▓▓▒▒░░░░░░░▒▒▒▒▒▒▒▒▒▒▒░░░ ░░░░░   ░
                             ▓███▓▓▓▓▓▒▓▒▒▒▒░░░░░░░░░▒▓▒▒░▀ ░░░  ░░░░░
                              ▀▒██▓▓▓▓▒▒▒▓▓▓▓▒▒▒▒▒▒▒▓▀▀░    ░░░░░░░░░     ░
                                 ▓▓▓▓▓▓▓▒▓▒▒▒▒▓▓▓▒▀░ ░░░░░▄░░░  ░░░  ░░░░░░
                                 ▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓     █▓▒░░▒░░░░ ░░░░░░░░
                                ▄▓▓▓▒▒▒▒▒░░░░░░░▒▄▄▄░▒▓▓▒▒░▀░
                               ░▓█▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▒░░░▒  besiyata
                               ▓▓█▓▓▒▓▓▓▒▒▒░░░░░░▒▓▓▓▓▒▒▒▒▒░   dishmaya
                               ▓▓█▓▓▓▓▓▓▒▒▒░░░░░░░▒▓▓▒▀▀▀
                               ▓▓██▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▀
                                █▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▀
                               ▒▓▓▓▓▀░░▒▓▓▓▓▓▓▓▓▒▒░░▒
                              ▄▓▓▀░░░▄▓▓▓▓▒▒▒▒▒░░░░▄░
                             ▄███▄▄▓▓▓▓▓▓▓▒▒▒▒▒░░▒▒░
                           ▄▓▓▓█▓█▓▓███▓▓▓▓▓▓▓▓▓▓▓░
                       ▄░▓▓▓▓▓▓▀▒▓▓▓▒▒▓▒░░░▒▓▒░░░▓
               ▄▄▄░▒▓▓▓▓▓▓░▀▀   ▓▓▒░▓▒▒▒▒▒▒▒▒▒▒▄░░▀▀░░ ▄▄▄▄
     ▄▄▄▒▒▓▓█▓▓▓▓▓▀▀▀▀▀        ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▀░░▀░░▒▒▒░░░   ░░░░░
  ▄▓▓▓▒▀▀                      ▓▒▓▓▓▓▓▒▒▒▒▒▒▒▒▓░░░       ▒▒▒░░░░░░░░▒
  █▓▓▒      ▄▄▄                  ▀▓▒▓▒▒▒▓▓▓▓▓▓▒▒▒░░░░░░░░░▒▒░░░░░░░
   ▀▓▓▓▓▒▄▄▒▒▒▒▒▒▄▄                    ▀▀▀▀░░▒▒▒▒░░░░░░
       ▀▀▀▓▓▓▓▒▒▒▒▒▓▓▄▄
╔────────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bell system five » system call support                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/	Performs System Five System Call.
/
/	Cosmopolitan is designed to delegate all function calls into the
/	Linux, FreeBSD, OpenBSD, and XNU kernels through this function,
/	with few exceptions. This function is intended to be called via
/	generated thunks in the libc/sysv/syscalls/ directory.
/
/	It's safe to call this function on Windows, where it'll always
/	return -1 w/ errno == ENOSYS. Further note that -1 is the only
/	return value that means error, a common anti-pattern is to check
/	for values less than 0 (which is more problematic on 32-bit).
/
/	It's important to consider that system calls are an order of a
/	magnitude more expensive than normal function calls. For example
/	getpid() on Linux usually takes 500ns, and cached i/o calls will
/	take 1µs or more.
/
/	@param	%rax function ordinal supplied by jump slot
/	@param	%rdi,%rsi,%rdx,%rcx,%r8,%r9 and rest on stack
/	@return	%rax:%rdx is result, or -1 w/ errno on error
/	@clob	%rcx,%r10,%r11
/	@see	syscalls.sh
	.initbss 300,_init_systemfive
hostos:	.quad	0
	.endobj	hostos,globl,hidden
systemfive:
	.quad	0
	.endobj	systemfive,globl,hidden
	.previous

.Lanchorpoint:
systemfive.linux:
	movswl	%ax,%eax
	test	%eax,%eax
	js	systemfive.enosys
	mov	%rcx,%r10
	syscall
	cmp	$-4095,%rax
	jae	systemfive.error
	ret
systemfive.error:
	neg	%eax
/	𝑠𝑙𝑖𝑑𝑒
systemfive.errno:
	mov	%eax,errno(%rip)
	pushpop	-1,%rax
	stc
	ret
systemfive.enosys:
	mov	ENOSYS(%rip),%eax
	jmp	systemfive.errno
systemfive.openbsd:
	shr	$48,%rax
	jmp	systemfive.bsd
systemfive.freebsd:
	shr	$32,%rax
	movzwl	%ax,%eax
/	𝑠𝑙𝑖𝑑𝑒
systemfive.bsd:
	cmp	$0xfff,%ax
	jae	systemfive.enosys
	mov	%rcx,%r10
	syscall
	jc	systemfive.errno
	ret
systemfive.xnu:
/	Do this
/	0x35022721530005
/	        │└┴┴─┐
/	        └┐   ├┬┐
/	0x00000002000153
	mov	%eax,%r11d
	shr	$0x10,%eax
	and	$0xfff,%eax
	shr	$0x1c,%r11d
	shl	$0x18,%r11d
	or	%r11d,%eax
	jmp	systemfive.bsd
	.endfn	systemfive.errno,globl,hidden
	.endfn	systemfive.error,globl,hidden
	.endfn	systemfive.enosys,globl,hidden
	.endfn	systemfive.xnu,globl,hidden
	.endfn	systemfive.freebsd,globl,hidden
	.endfn	systemfive.openbsd,globl,hidden
	.endfn	systemfive.linux,globl,hidden
	.previous

/	Initializes System Five system call support.
/
/	  (1) Extracts parameters passed by kernel,
/	  (2) Detects O/S without issuing system calls,
/	  (3) Unpacks numbers.
/
/	@param	%r15 is auxv
/	@note	OpenBSD devs: let us know if you start using auxv
	.init.start 300,_init_systemfive
	push	%rbx
	push	%rsi
	testb	$XNU,(%rdi)			# @see libc/crt/crt.S
	jnz	.Lsystemfive.init.xnu
	testb	$FREEBSD,(%rdi)			# @see libc/crt/crt.S
	jnz	.Lsystemfive.init.freebsd
	testb	$WINDOWS,(%rdi)			# @see libc/runtime/winmain.c
	jnz	.Lsystemfive.init.windows
	cmpq	$0,(%r15)			# OpenBSD doesn't have auxv
	je	.Lsystemfive.init.openbsd
/	default state is safe state
/	𝑠𝑙𝑖𝑑𝑒
.Lsystemfive.init.linux:
	pushb	systemfive.linux-.Lanchorpoint
	push	$LINUX
	ezlea	syscon.linux,si
	jmp	.Lsystemfive.init.os
.Lsystemfive.init.windows:
	pushb	systemfive.enosys-.Lanchorpoint
	push	$WINDOWS
	ezlea	syscon.windows,si
	jmp	.Lsystemfive.init.os
.Lsystemfive.init.freebsd:
	pushb	systemfive.freebsd-.Lanchorpoint
	push	$FREEBSD
	ezlea	syscon.freebsd,si
	jmp	.Lsystemfive.init.os
.Lsystemfive.init.openbsd:
	pushb	systemfive.openbsd-.Lanchorpoint
	push	$OPENBSD
	ezlea	syscon.openbsd,si
	jmp	.Lsystemfive.init.os
.Lsystemfive.init.xnu:
	pushb	systemfive.xnu-.Lanchorpoint
	push	$XNU
	ezlea	syscon.xnu,si
/	𝑠𝑙𝑖𝑑𝑒
.Lsystemfive.init.os:
	ezlea	.Lanchorpoint,cx
	pop	%rax
	stosq	#→ hostos
	pop	%rax
	add	%rcx,%rax
	stosq	#→ systemfive
.Lsystemfive.sleb128unpacker:
	push	%rdi
	ezlea	.Lsyscon.start,di
	orq	$-1,%r9
	ezlea	.Lsyscon.end,bx
2:	cmp	%rbx,%rdi
	jnb	5f
	xor	%ecx,%ecx
	xor	%edx,%edx
3:	lodsb
	mov	%rax,%r8
	and	$127,%r8d
	sal	%cl,%r8
	add	$7,%ecx
	or	%r8,%rdx
	test	%al,%al
	js	3b
	test	$64,%al
	je	4f
	mov	%r9,%rax
	sal	%cl,%rax
	or	%rax,%rdx
4:	mov	%rdx,%rax
	cmpq	$0,(%rdi)		# don't change consts already set
	cmovne	(%rdi),%rax		# @see WinMain() for example
	stosq
	jmp	2b
5:	pop	%rdi
/	𝑠𝑙𝑖𝑑𝑒
.Lsystemfive.init.done:
	pop	%rsi
	pop	%rbx
	.init.end 300,_init_systemfive,globl,hidden

/	Sections for varint encoded numbers.
/
/	These sections are all ordered by (group_name, constant_name).
/	They're populated by modules simply referencing the symbols.
/
/	@see libc/sysv/consts.sh
/	@see libc/sysv/consts/syscon.h
	.section .piro.bss.sort.syscon.1,"aw",@nobits
	.align	8
.Lsyscon.start:/*
	...decentralized quadwords...
	*/.previous
	.section .piro.bss.sort.syscon.3,"aw",@nobits
.Lsyscon.end:
	.previous
	.section .sort.rodata.syscon.linux.1,"a",@progbits
	.align	1
syscon.linux:/*
	...decentralized leb128...
	*/.previous
	.section .sort.rodata.syscon.xnu.1,"a",@progbits
	.align	1
syscon.xnu:/*
	...decentralized leb128...
	*/.previous
	.section .sort.rodata.syscon.freebsd.1,"a",@progbits
	.align	1
syscon.freebsd:/*
	...decentralized leb128...
	*/.previous
	.section .sort.rodata.syscon.openbsd.1,"a",@progbits
	.align	1
syscon.openbsd:/*
	...decentralized leb128...
	*/.previous
	.section .sort.rodata.syscon.windows.1,"a",@progbits
	.align	1
syscon.windows:/*
	...decentralized leb128...
	*/.previous
	.type	.Lsyscon.start,@object
	.type	.Lsyscon.end,@object
	.type	syscon.linux,@object
	.type	syscon.xnu,@object
	.type	syscon.freebsd,@object
	.type	syscon.openbsd,@object
	.type	syscon.windows,@object
