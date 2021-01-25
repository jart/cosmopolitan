/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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

	.initbss 300,_init_systemfive
__hostos:
	.quad	0
	.endobj	__hostos,globl,hidden

/	Performs System Five System Call.
/
/	Cosmopolitan is designed to delegate all function calls into the
/	Linux, FreeBSD, OpenBSD, and XNU kernels via this function, with
/	few exceptions. This function should generally only be called by
/	generated thunks in the libc/sysv/syscalls/ directory.
/
/	It's safe to call this function on Windows, where it will always
/	return -1 with errno == ENOSYS. Further note that -1 is the only
/	return value that means error, a common anti-pattern is to check
/	for values less than 0 (which is more problematic on 32-bit).
/
/	It is important to consider that system calls are one order of a
/	magnitude more expensive than normal function calls. For example
/	getpid() on Linux usually takes 500ns, and cached i/o calls will
/	take 1µs or more. So we don't need to inline them like Chromium.
/
/	Another thing to consider is that BSDs only loosely follow the
/	System Five ABI for the SYSCALL instruction. For example Linux
/	always follows the six argument limit but the FreeBSD sendfile
/	system call accepts a seventh argument that is passed on stack
/	and OpenBSD modifies functions like mmap so that the sixth arg
/	is passed on the stack. There's also the carry flag convention
/	that XNU, FreeBSD, and OpenBSD inherited from 386BSD aka Jolix
/
/	@param	%rax function ordinal supplied by jump slot
/	@param	%rdi,%rsi,%rdx,%rcx,%r8,%r9 and rest on stack
/	@return	%rax:%rdx is result, or -1 w/ errno on error
/	@clob	%rcx,%r10,%r11
/	@see	syscalls.sh
__systemfive:
	.quad	0
	.endobj	__systemfive,globl,hidden
	.previous
.Lanchorpoint:
systemfive.linux:
	movswl	%ax,%eax		# gnu/systemd ordinal is first word
	test	%eax,%eax		# sign extend to optimize code size
	js	systemfive.enosys	# test for syscalls.sh ordinal ffff
	mov	%rcx,%r10		# syscall instruction clobbers %rcx
	push	%rbp			# linux never reads args from stack
	mov	%rsp,%rbp		# having frame will help backtraces
	syscall				# this is known as a context switch
	pop	%rbp			# next we check to see if it failed
	cmp	$-4095,%rax		# system five nexgen32e abi § A.2.1
	jae	systemfive.error	# encodes errno as neg return value
	ret
	.endfn	systemfive.linux,globl,hidden
systemfive.error:
	neg	%eax
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	systemfive.error,globl,hidden
systemfive.errno:
	mov	%eax,errno(%rip)	# normalize to c library convention
	push	$-1			# negative one is only error result
	pop	%rax			# the push pop is to save code size
	ret
	.endfn	systemfive.errno,globl,hidden
systemfive.enosys:
	mov	ENOSYS(%rip),%eax
	jmp	systemfive.errno
	.endfn	systemfive.enosys,globl,hidden
systemfive.openbsd:
	shr	$48,%rax
	jmp	systemfive.bsd
	.endfn	systemfive.openbsd,globl,hidden
systemfive.freebsd:
	shr	$32,%rax
	movzwl	%ax,%eax
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	systemfive.freebsd,globl,hidden
systemfive.bsd:
	cmp	$0xfff,%ax
	jae	systemfive.enosys
	mov	%rcx,%r10		# note: we do not create a stack frame
	syscall				# bsd will need arg on stack sometimes
	jc	systemfive.errno	# bsd sets carry flag if %rax is errno
	ret
	.endfn	systemfive.bsd
systemfive.xnu:
/	0x????????2153????		# how syscalls.sh encodes xnu ordinals
/	          │└┴┴─┐
/	          └┐   ├┬┐
/	0x0000000002000153		# how xnu wants ordinals to be encoded
	mov	%eax,%r11d
	shr	$4*7,%r11d
	shl	$4*6,%r11d
	shl	$4*1,%eax
	shr	$4*5,%eax
	or	%r11d,%eax
	jmp	systemfive.bsd
	.endfn	systemfive.xnu,globl,hidden
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
	testb	$METAL,(%rdi)			# @see ape/ape.S
	jnz	systemfive.init.metal
	testb	$XNU,(%rdi)			# @see libc/crt/crt.S
	jnz	systemfive.init.xnu
	testb	$FREEBSD,(%rdi)			# @see libc/crt/crt.S
	jnz	systemfive.init.freebsd
	testb	$WINDOWS,(%rdi)			# @see libc/runtime/winmain.c
	jnz	systemfive.init.windows
	cmpq	$0,(%r15)			# OpenBSD doesn't have auxv
	je	systemfive.init.openbsd
/	default state is safe state
/	𝑠𝑙𝑖𝑑𝑒
systemfive.init.linux:
	pushb	systemfive.linux-.Lanchorpoint
	push	$LINUX
	ezlea	syscon.linux,si
	jmp	systemfive.init.os
systemfive.init.metal:
	pushb	systemfive.linux-.Lanchorpoint
	push	$METAL
	ezlea	syscon.linux,si
	jmp	systemfive.init.os
systemfive.init.windows:
	pushb	systemfive.enosys-.Lanchorpoint
	push	$WINDOWS
	ezlea	syscon.windows,si
	jmp	systemfive.init.os
systemfive.init.freebsd:
	pushb	systemfive.freebsd-.Lanchorpoint
	push	$FREEBSD
	ezlea	syscon.freebsd,si
	jmp	systemfive.init.os
systemfive.init.openbsd:
	pushb	systemfive.openbsd-.Lanchorpoint
	push	$OPENBSD
	ezlea	syscon.openbsd,si
	jmp	systemfive.init.os
systemfive.init.xnu:
	pushb	systemfive.xnu-.Lanchorpoint
	push	$XNU
	ezlea	syscon.xnu,si
/	𝑠𝑙𝑖𝑑𝑒
systemfive.init.os:
	ezlea	.Lanchorpoint,cx
	pop	%rax
	stosq	#→ __hostos
	pop	%rax
	add	%rcx,%rax
	stosq	#→ __systemfive
	push	%rdi
	ezlea	syscon.start,di
	ezlea	syscon.end,bx
	call	systemfive.sleb128unpacker
	pop	%rdi
/	𝑠𝑙𝑖𝑑𝑒
systemfive.init.done:
	pop	%rsi
	pop	%rbx
	.init.end 300,_init_systemfive,globl,hidden

	.text.startup
systemfive.sleb128unpacker:
	.leafprologue
	or	$-1,%r9
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
5:	.leafepilogue
	.previous

/	Sections for varint encoded numbers.
/
/	These sections are all ordered by (group_name, constant_name).
/	They're populated by modules simply referencing the symbols.
/
/	@see libc/sysv/consts.sh
/	@see libc/sysv/consts/syscon.h
	.section .piro.bss.sort.syscon.1,"aw",@nobits
	.align	8
syscon.start:/*
	...decentralized quadwords...
	*/.previous
	.section .piro.bss.sort.syscon.3,"aw",@nobits
syscon.end:
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

	.type	syscon.start,@object
	.type	syscon.end,@object
	.type	syscon.linux,@object
	.type	syscon.xnu,@object
	.type	syscon.freebsd,@object
	.type	syscon.openbsd,@object
	.type	syscon.windows,@object

	.globl	syscon.start
	.globl	syscon.end
	.globl	syscon.linux
	.globl	syscon.xnu
	.globl	syscon.freebsd
	.globl	syscon.openbsd
	.globl	syscon.windows
